#include "header.h"
#define bmpLink "anime32Bit.bmp"

int main(int argc, char* argv[]) {

	bmpFile bmp1, bmp2, dst1, dst2;
	string a1(argv[1]);
	if (a1 == "-conv") {
		readBmpFile(argv[2], bmp1);
		convertTo8Bit(bmp1, dst1);
		writeBmpFile(argv[3], dst1);
	}
	else if (a1 == "-zoom") {
		readBmpFile(argv[2], bmp2);
		//convertCharToInt(argv[4], S);
		int S = atoi(argv[4]);
		zoomOutPixel(bmp2, dst2, S);
		writeBmpFile(argv[3], dst2);
	}

	return 0;
}