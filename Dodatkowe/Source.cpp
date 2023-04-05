#define _USE_MATH_DEFINES
#include "intrin.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <complex>

using namespace std;

complex<float> operator/(const complex<float>& a, int x) {
	return complex<float>(a.real()/x, a.imag()/x);
}

class fmdemod {
private:
	std::vector<complex<float>> arr;
public:
	float demodulate() {
		float y = arg(arr[1] * conj(arr[0])) / M_PI;
		arr[0] = arr[1];
		arr.pop_back();
		return y;
	}
	
	void add(complex<float> p) {
		arr.push_back(p);
	}

	size_t getSize() {
		return arr.size();
	}
};

template<typename T, int K>
class decimate {

private:
	std::vector<T> arr;
public:
	decimate()
	{
		arr.reserve(K);
	}

	void add(T p) {
		arr.push_back(p);
	}

	T average() {
		T result = 0;
		for (int i = 0; i < K; i++) {
			result += arr[i] / K;
		}
		return result;
	}
	
	size_t getSize() {
		return arr.size();
	}

	void clear() {
		arr.clear();
	}
};

class fmshift {
private:	
	const float p = -0.090625;
public:
	complex<float> shift(complex<float> x, int n) {
		return complex<float>(x * exp(complex<float>(0, 2 * M_PI * p * n)));
	}
};

class cu8data {
private:
	fmshift fm;
	decimate<complex<float>, 5> dec1;
	fmdemod demod;
	decimate<float, 8> dec2;
public:
	void handleData(std::string plik, std::string au) {
		ifstream myfile(plik, ios::binary);
		ofstream write(au, std::ios::out | std::ios::binary);
		int a = 0x2e736e64;
		a = _byteswap_ulong(a);
		write.write(reinterpret_cast<const char*>(&a), sizeof(a));
		int b = 24;
		b = _byteswap_ulong(b);
		write.write(reinterpret_cast<const char*>(&b), sizeof(b));
		int c = 0xffffffff;
		c = _byteswap_ulong(c);
		write.write(reinterpret_cast<const char*>(&c), sizeof(c));
		int d = 6;
		d = _byteswap_ulong(d);
		int e = 48000;
		e = _byteswap_ulong(e);
		int f = 1;
		f = _byteswap_ulong(f);
		write.write(reinterpret_cast<const char*>(&d), sizeof(d));
		write.write(reinterpret_cast<const char*>(&e), sizeof(e));
		write.write(reinterpret_cast<const char*>(&f), sizeof(f));

		int n = 0;
		while (true) {
			myfile.seekg(2 * n);
			char r = myfile.get();
			char im = myfile.get();
			if (r == EOF || im == EOF) break;

			complex<float> x((r - 127.5) / 127.5, ((im - 127.5) / 127.5));
			x = fm.shift(x, n);
			dec1.add(x);
			if (dec1.getSize() == 5) {
				complex<float> avg = dec1.average();
				demod.add(avg);
				if (demod.getSize() == 2) {
					dec2.add(demod.demodulate());
					if (dec2.getSize() == 8) {
						float y = dec2.average();
						write.write(reinterpret_cast<const char*>(&y), sizeof(y));
						dec2.clear();
					}
				}
				dec1.clear();
			}
			n++;
		}
	}
};

int main() {
	cu8data data;
	data.handleData("fm1_99M726_1M92.cu8", "result.au");
	return 0;
}
