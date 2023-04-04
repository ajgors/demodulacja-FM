#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#include "intrin.h"
#include <bit>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <complex>
#include <array>
#include <list>
#include <cmath>
using namespace std;


class cu8data {

public:
	void wczytaj(std::string plik, std::string au) {
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
			//x = complex<float>(x * exp(complex<float>(0, 2 * M_PI * p * n)));
			//decimation1.push_back(x);
			n++;
		}
	}
};


int main() {
	cu8data data;
	data.wczytaj("fm1_99M726_1M92.cu8", "result.au")
	/*string line;
	ifstream myfile("fm1_99M726_1M92.cu8", ios::binary);
	ofstream write("result.au", std::ios::out | std::ios::binary);
	std::cout << sizeof(float) << std::endl;
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
	write.write(reinterpret_cast<const char*>(&f), sizeof(f));*/
	
	if (myfile.is_open())
	{
		const float p = -0.090625;
		int n = 0;
		vector<complex<float>> decimation1;
		decimation1.reserve(5);
		vector<float> decimation2;
		decimation2.reserve(8);
		vector<complex<float>> arr;
		while (true) {
			if (decimation1.size() == 5) {
				complex<float> newC;
				for (int i = 0; i < 5; i++) {
					newC += complex<float>(decimation1[i].real()/5, decimation1[i].imag()/5);
				}
				decimation1.clear();
				arr.push_back(newC);

				if (arr.size() == 2) {
					float y = arg(arr[1]*conj(arr[0]))/ M_PI;
					arr[0] = arr[1];
					arr.pop_back();
					decimation2.push_back(y);
				}
			}


			if (decimation2.size() == 8) {
				float newC = 0;
				for (int i = 0; i < 8; i++) {
					newC += decimation2[i]/8;
				}
				write.write(reinterpret_cast<const char*>(&newC), sizeof(float));
				decimation2.clear();
			}


			myfile.seekg(2 *n);
			char r = myfile.get();
			char im = myfile.get();
			if (r == EOF || im == EOF) break;

			complex<float> x((r-127.5)/127.5, ((im-127.5)/127.5));
			x = complex<float>(x * exp(complex<float>(0, 2*M_PI*p*n)));
			decimation1.push_back(x);
			n++;
		}
	}
	else {
	}

	return 0;
}
