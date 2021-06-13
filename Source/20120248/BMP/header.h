#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string.h>
#include <fstream>
#pragma pack(1)
using namespace std;

//kieu du lieu trong file header
struct Header {
	unsigned char signature[2];
	uint32_t fileSizeBmp;
	uint32_t reserved;
	uint32_t dataOffset;
};

//kieu du lieu trong file dib
struct Dib {
	uint32_t dibSize;
	uint32_t width;
	uint32_t height;
	uint16_t planesColor;
	uint16_t bitsPerPixel;
	uint32_t compressMethod;
	uint32_t imageSize;
	uint32_t horResolution;
	uint32_t verResolution;
	uint32_t colorsUsed;
	uint32_t importantColor;
};

struct ColorTable {
	unsigned char B;//blue
	unsigned char G;//green
	unsigned char R;//red
	unsigned char A;//Alpha
};

struct BGR {	
	unsigned char B;//blue
	unsigned char G;//green
	unsigned char R;//red
};

struct pPixel {
	unsigned char* dataArr;
	BGR** pixelArr_24Bit;
	ColorTable** pixelArr_32Bit;
};

struct bmpFile {
	Header headerBmp;
	Dib dibBmp;
	char* pDibReserved;
	ColorTable* colorTable;
	pPixel pPixelData;
};	

int readBmpFile(char*, bmpFile&);
int writeBmpFile(char*&, bmpFile);
void printBmp(bmpFile);
void convertTo8Bit(bmpFile, bmpFile&);
void zoomOutPixel(bmpFile, bmpFile&, int);
