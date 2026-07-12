#pragma once
#pragma once
#include <iostream>
#include <fstream>
#include <cmath>

class Complex;
class Filter;
class Spectrum;
class Whisper;

#pragma pack(push, 1)
struct WavHeader {
	char chunkId[4];
	int chunkSize;
	char format[4];
	char subchunk1Id[4];
	int subchunk1Size;
	short audioFormat;
	short numChannels;
	int sampleRate; //4410
	int byteRate;
	short blockAlign;
	short bitsPerSample;
	char subchunk2Id[4];
	int subchunk2Size;
};
#pragma pack(pop)


class Complex {
public:
	float r, i;
	Complex();
	Complex(float r, float i);
	Complex operator+(const Complex& cpy);
	Complex operator*(const Complex& cpy);
	Complex operator-(const Complex& cpy);
};

class Filter {
public:
	Complex* w; //dynamic array pointing to floats
	int sizeOfW; //size of dynamic array
	int smpleRate; //sample rate 

	//constructor, destructor, and copy constructor
	Filter();
	~Filter();
	Filter(int size_w, int smplRate);
	Filter(const Filter& cpy);
	Filter& operator= (const Filter& cpy);
	void bpFilter(float lowlim, float highlim);
	void lpFilter(float highlim);
	void hpFilter(float lowlim);
};

class Spectrum {
public:
	Complex* fBin; //dyanmic array -- freq bins - holds real and imag values
	int oSize; //original size without the padded zeroes
	int pSize; //padded size which is the power of 2 size
	int smplRate; //sample rate
	Spectrum();
	Spectrum(int oSize, int pSize, int smplRate);
	~Spectrum(); //destructor
	//for the dynamic pointers
	Spectrum& operator=(const Spectrum& cpy);
	Spectrum(const Spectrum& cpy);
	Whisper i_FFT();
	void filteration(Filter& f);
};

class Whisper {
public:
	WavHeader hdr;
	short* audioStr;
	int numOfSamples;
	Whisper();
	~Whisper();
	Whisper(const char* file);
	Whisper(const Whisper& other);
	Whisper& operator=(const Whisper& other);
	void audReversal();
	Whisper& operator*=(float gain);
	Whisper operator+(const Whisper& other);
	Whisper& operator+=(const Whisper& other);
	void pNormal();
	Whisper splice(float s, float e);
	Spectrum fwdTransform();


void downld(const char* n);
private:
	void loadinc(Spectrum& s);

};