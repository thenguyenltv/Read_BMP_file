#include "header.h"

//Thiet ke mang dong 2 chieu luu pixel data
void getPixelArr(bmpFile& bmp) {
	//so dong = height
	int rowCount = bmp.dibBmp.height;
	//so dong = width
	int columnCount = bmp.dibBmp.width;
	//so byte trong 1 pixel
	int BytePerPixel = bmp.dibBmp.bitsPerPixel / 8;
	//so byte co moi padding
	int padding = (4 - ((bmp.dibBmp.width * BytePerPixel) % 4)) % 4;
	//so byte tren moi hang
	int byteInRow = BytePerPixel * bmp.dibBmp.width + padding;

	//anh 24 bit
	if (bmp.dibBmp.bitsPerPixel == 24) {
		//cap phat vung nho cho mang BGR
		bmp.pPixelData.pixelArr_24Bit = new BGR * [rowCount];
		for (int i = 0; i < rowCount; i++) {
			bmp.pPixelData.pixelArr_24Bit[i] = (BGR*)(bmp.pPixelData.dataArr + byteInRow*i);
		}
	}//anh 32 bit
	else if (bmp.dibBmp.bitsPerPixel == 32) {
		//cap phat vung nho mang BGRA
		bmp.pPixelData.pixelArr_32Bit = new ColorTable * [rowCount];
		for (int i = 0; i < rowCount; i++) {
			bmp.pPixelData.pixelArr_32Bit[i] = (ColorTable*)(bmp.pPixelData.dataArr + byteInRow * i);
		}
	}
}

int readBmpFile(char* fileName, bmpFile& bmp) {
	fstream f;
	f.open(fileName, ios::in | ios::binary);
	if (!f) {
		cout << "File opening failed" << "\n";
		return 0;
	}

	//Doc file header va file Dib
	f.seekg(0, ios::beg);
	f.read((char*)&bmp.headerBmp, 14);
	f.read((char*)&bmp.dibBmp, 40);

	//doc du lieu phan thua
	if (bmp.dibBmp.dibSize > 40) {
		//kich thuoc cua phan du
		int sizeofReserved = bmp.dibBmp.dibSize - 40;
		//cap phat vung nho cho phan du Dib
		bmp.pDibReserved = new char[sizeofReserved];
		//Doc phan du
		f.read((char*)bmp.pDibReserved, sizeofReserved);
	}
	
	//Doc Color table (neu co)
	int nColorTable = bmp.headerBmp.dataOffset - (bmp.dibBmp.dibSize + 14);
	if (nColorTable > 0) {
		bmp.colorTable = new ColorTable[nColorTable/4];
		f.read((char*)bmp.colorTable, nColorTable);
	}

	//doc du lieu diem anh
	bmp.pPixelData.dataArr = new unsigned char[bmp.dibBmp.imageSize];
	f.read((char*)bmp.pPixelData.dataArr, bmp.dibBmp.imageSize);
	
	//Chuyển dữ liệu sang mảng 2 chiều
	getPixelArr(bmp);

	//dong file
	f.close();
	return 1;
}

void printHeader(bmpFile bmp) {
	Header tmp = bmp.headerBmp;

	cout << "\tInformation of BMP file" << "\n";
	cout << "***BMP Header***" << "\n";
	cout << "- Signature: " << tmp.signature[0] << tmp.signature[1] << "\n";
	cout << "- fileSize: " << tmp.fileSizeBmp << "\n";
	cout << "- Reserved: " << tmp.reserved << "\n";
	cout << "- Data Offset: " << tmp.dataOffset << "\n";
}

void printDib(bmpFile bmp) {
	Dib tmp = bmp.dibBmp;
	cout << "***BMP Dib***" << "\n";
	cout << "- Dib Size: " << tmp.dibSize << "\n";
	cout << "- Width: " << tmp.width << "\n";
	cout << "- Height: " << tmp.height << "\n";
	cout << "- Color planes: " << "" << tmp.planesColor << "\n";
	cout << "- Bpp: " << tmp.bitsPerPixel << "\n";
	cout << "- Compression: " << tmp.compressMethod << "\n";
	cout << "- Image Size: " << tmp.imageSize << "\n";
	cout << "- X_pixel/m: " << tmp.horResolution << "\n";
	cout << "- Y_pixel/m: " << tmp.verResolution << "\n";
	cout << "- Colors used: " << tmp.colorsUsed << "\n";
	cout << "- Important color: " << tmp.importantColor << "\n";
}

void printBmp(bmpFile bmp) {
	printHeader(bmp);
	printDib(bmp);
}

int writeBmpFile(char*& fileName, bmpFile bmp) {
	ofstream f(fileName, ios::out | ios::binary);
	if (!f) {
		cout << "File opening failed" << "\n";
		return 0;
	}

	//Ghi header, Dib
	f.seekp(0, ios::beg);
	f.write((char*)&bmp.headerBmp, 14);
	f.write((char*)&bmp.dibBmp, 40);

	//Ghi du lieu phan thua
	if (bmp.dibBmp.dibSize > 40) {
		//kich thuoc cua phan du
		int sizeofReserved = bmp.dibBmp.dibSize - 40;
		//Ghi phan du
		f.write((char*)bmp.pDibReserved, sizeofReserved);
	}

	//Ghi Color table (neu co)
	int nColorTable = bmp.headerBmp.dataOffset - (bmp.dibBmp.dibSize + 14);
	if (nColorTable > 0) {
		f.write((char*)bmp.colorTable, nColorTable);
	}

	//Ghi du lieu diem anh
	f.write((char*)bmp.pPixelData.dataArr, bmp.dibBmp.imageSize);

	f.close();
	return 1;
}

//chuyen doi tu anh begin(24/32bit) sang anh finish 8 bit
void convertTo8Bit(bmpFile begin, bmpFile& finish) {
	//lưu header, dib, phần dư finish = begin
	finish.headerBmp = begin.headerBmp;
	finish.dibBmp = begin.dibBmp;
	finish.pDibReserved = begin.pDibReserved;

	int height = begin.dibBmp.height;
	int width = begin.dibBmp.width;
	int padding8Bit = (4 - ((width) % 4)) % 4;//(width*(bpp/8) + padding) % 4 = 0
	//thay đổi các giá trị quan trọng
	finish.dibBmp.bitsPerPixel = 8;	
	finish.headerBmp.dataOffset = begin.headerBmp.dataOffset + 1024;//1024 bytes(256 color * 4 bytes)
	finish.dibBmp.imageSize = padding8Bit * height + width * height;
	finish.headerBmp.fileSizeBmp = finish.dibBmp.imageSize + finish.headerBmp.dataOffset;//size of Image + a number of bytes to Data Offset	
	
	//get data of Color table!!!(BGRA --> 0-255)
	finish.colorTable = new ColorTable[256];
	for (int i = 0; i < 256; i++) {
		finish.colorTable[i].B = i;
		finish.colorTable[i].G = i;
		finish.colorTable[i].R = i;
		finish.colorTable[i].A = i;
	}

	//cấp phát bộ nhớ cho mảng dataArr
	finish.pPixelData.dataArr = new unsigned char[finish.dibBmp.imageSize];
	//number of row = height
	int rowCount = height;
	//number of column = width
	int columnCount	= width;
	int count = 0;
	for (int i = 0; i < rowCount; i++){
		for (int j = 0; j < columnCount; j++) {
			unsigned char avg;
			if (begin.dibBmp.bitsPerPixel == 24) {//24 bit
				BGR tmp = begin.pPixelData.pixelArr_24Bit[i][j];
				avg = tmp.G / 3 + tmp.B / 3 + tmp.R / 3;
			}
			else { //32 bit
				ColorTable tmp = begin.pPixelData.pixelArr_32Bit[i][j];
				avg = tmp.G / 3 + tmp.B / 3 + tmp.R / 3;
			}
			finish.pPixelData.dataArr[count++] = avg;
		}
		count += padding8Bit;
	}
}

void convertInforZoom(bmpFile scr, bmpFile& dst, int S) {
	//convert information of Header & Dib from scr to dst
	dst.headerBmp = scr.headerBmp;
	dst.dibBmp = scr.dibBmp;
	dst.pDibReserved = scr.pDibReserved;
	dst.colorTable = scr.colorTable;

	//change important information
	int height = scr.dibBmp.height;
	int width = scr.dibBmp.width;
	int bytePerPix = scr.dibBmp.bitsPerPixel / 8;
	dst.dibBmp.width = width / S;
	dst.dibBmp.height = height / S;
	//nếu có phần dư với kích thước < S
	if (width % S != 0)
		dst.dibBmp.width++;
	if (height % S != 0)
		dst.dibBmp.height++;
	int padding = (4 - ((dst.dibBmp.width *  bytePerPix) % 4)) % 4;//(width*(bpp/8) + padding) % 4 = 0
	//new Image size = New Pixel Array + new padding
	dst.dibBmp.imageSize = dst.dibBmp.width * dst.dibBmp.height * bytePerPix + padding * dst.dibBmp.height;
	dst.headerBmp.fileSizeBmp = dst.headerBmp.dataOffset + dst.dibBmp.imageSize;
}

int avgPixelValue(int* p, int n) {
	int sum = 0;
	for (int i = 0; i < n; i++) {
		sum += p[i];
	}
	return sum / n;
}

void createPixelData(bmpFile scr, bmpFile& dst, int S) {
	//Khai bao cac gia tri quan trong
	int rowCount = scr.dibBmp.height;
	int columnCount = scr.dibBmp.width;
	int bytePerPix = dst.dibBmp.bitsPerPixel / 8;
	int padding = (4 - (columnCount * bytePerPix) % 4) % 4;//padding của mảng pixel ảnh scr
	int paddingZoom = (4 - (dst.dibBmp.width * bytePerPix) % 4) % 4;//padding của mảng pixel ảnh dst
	int byteInRowSCR = columnCount * bytePerPix + padding;//So byte trong 1 hang cua anh scr
	int count = 0;//đếm số ô đã đi được trong S x S
	int index = 0;//chỉ số trên mảng dataArr của dst	
	int m = S, n = S;//kích thước có thế thay đổi trên các dòng và cột cuối
	int* pBlue = new int[m*n];
	int* pGreen = new int[m*n];
	int* pRed = new int[m*n];
	int* pColor8Bit = new int[m*n];
	int xWidth = 0, yHeight = 0;
	

	//i chay theo chieu dai kich thuoc S
	for (int i = yHeight; i < yHeight + m; i++) {
		//Neu i dong cuoi cung --> cap nhat lai chieu dai S
		if (i == rowCount - 1) {
			//cout << "i:" << i;
			m = rowCount - yHeight;
		}
		//j chay theo chieu rong kich thuoc S
		for (int j = xWidth; j < xWidth + n; j++) {
			if (dst.dibBmp.bitsPerPixel == 24) {
				BGR tmp = scr.pPixelData.pixelArr_24Bit[i][j];
				pBlue[count] = tmp.B;
				pGreen[count] = tmp.G;
				pRed[count] = tmp.R;
			}
			else if (dst.dibBmp.bitsPerPixel == 32) {
				ColorTable tmp = scr.pPixelData.pixelArr_32Bit[i][j];
				pBlue[count] = tmp.B;
				pGreen[count] = tmp.G;
				pRed[count] = tmp.R;
				int alpha = index + 3;
				dst.pPixelData.dataArr[alpha] = tmp.A;
			}
			else if (dst.dibBmp.bitsPerPixel == 8) {
				pColor8Bit[count] = scr.pPixelData.dataArr[i * byteInRowSCR + j];
			}
			count++;
			//Neu j den cot cuoi cung --> cap nhat lai chieu rong S
			if (xWidth + n >= columnCount)
				if (j == columnCount - 1) {
					//Gan lai gia tri cua n neu xWidth + n > columnCount
					n = columnCount - xWidth;
					//Neu i o dong cuoi cung cua o kich thuoc S x S
					if (i == (yHeight + m - 1)) {
						yHeight += m;
						xWidth = -n;
						//Cong them phan padding (neu co)
						index += paddingZoom;
						break;
					}
				}
		}
		//Khi count di den o cuoi cung cua o kich thuoc S x S
		if (count == (m*n)) {
			//Gan cac gia tri dataArr[] = avg cua B, G va R
			if (dst.dibBmp.bitsPerPixel == 8) {
				dst.pPixelData.dataArr[index++] = avgPixelValue(pColor8Bit, m * n);
			}
			else {
				dst.pPixelData.dataArr[index++] = avgPixelValue(pBlue, m * n);
				dst.pPixelData.dataArr[index++] = avgPixelValue(pGreen, m * n);
				dst.pPixelData.dataArr[index++] = avgPixelValue(pRed, m * n);
				int aa = avgPixelValue(pRed, m * n);
				//anh 32 bit Color Table: BGRA
				if (dst.dibBmp.bitsPerPixel == 32)
					index++;
			}
			count = 0;
			xWidth += n;
			i = yHeight - 1;
			n = S;
		}
		if (index == dst.dibBmp.imageSize)
			break;
	}
	delete[] pBlue;
	delete[] pGreen;
	delete[] pRed;
	delete[] pColor8Bit;
}

void zoomOutPixel(bmpFile scr, bmpFile& dst, int S) {

	//Write data for Header & Dib bmpFile dst
	convertInforZoom(scr, dst, S);
	//cap phat vung nho cho mang luu Pixel Data
	dst.pPixelData.dataArr = new unsigned char[dst.dibBmp.imageSize];
	//Create Pixel Data 
	createPixelData(scr, dst, S);
}
