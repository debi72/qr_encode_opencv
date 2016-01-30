#ifndef __MAIN_QR_SERVER_H__
#define __MAIN_QR_SERVER_H__

#include <string.h>
#include <errno.h>
#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <wchar.h>
#include <baseapi.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <allheaders.h>
#include "qrencode.h"

using namespace tesseract;

//TCP Socker defines




// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "457"
#define DEFAULT_IP "localhost"

WSADATA wsaData;
int iResult;

SOCKET ListenSocket = INVALID_SOCKET;
SOCKET ClientSocket = INVALID_SOCKET;

struct addrinfo* result = NULL;
struct addrinfo hints;

int iSendResult;
char recvbuf[DEFAULT_BUFLEN];
int recvbuflen = DEFAULT_BUFLEN;

int InitConnection();
int CloseConnection();


//tesseract defines

#define OUT_FILE_PIXEL_PRESCALER	11		// Prescaler (number of pixels in bmp file for each QRCode pixel, on each dimension)

#define PIXEL_COLOR_R				0		// Color of bmp pixels
#define PIXEL_COLOR_G				0
#define PIXEL_COLOR_B				0

#define QR_CODE_WINDOW "QR Code"
#define BMP_EXT ".bmp"

typedef unsigned short	WORD;
typedef unsigned long	DWORD;
typedef signed long		LONG;

#define BI_RGB			0L

#pragma pack(push, 2)

typedef struct
{
	WORD    bfType;
	DWORD   bfSize;
	WORD    bfReserved1;
	WORD    bfReserved2;
	DWORD   bfOffBits;
} BMPFILEHEADER;

typedef struct
{
	DWORD      biSize;
	LONG       biWidth;
	LONG       biHeight;
	WORD       biPlanes;
	WORD       biBitCount;
	DWORD      biCompression;
	DWORD      biSizeImage;
	LONG       biXPelsPerMeter;
	LONG       biYPelsPerMeter;
	DWORD      biClrUsed;
	DWORD      biClrImportant;
} BMPINFOHEADER;

#pragma pack(pop)

enum QRErrors
{
	QR_OK, QR_OUT_OF_MEMORY, QR_NULL_POINTER, QR_UNABLE_OPEN_FILE
};



char* concat(char* fisrt, char* second)
{
	char* res;
	res = (char* )malloc(strlen(fisrt)+strlen(second)+1);
	strcpy(res, fisrt); 
	strcat(res, second); 
	return res;
}

char* IntToStr(int a){
   char* p;
   p = (char*) malloc(sizeof(char)*33);
   itoa(a,p,10);
   return p;
}

struct Text {
	char* str;
	int len;
};

char* Decode (char*);
QRErrors QREncode(char*, char*);
char* alph = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_";

#endif /* __MAIN_QR_SERVER_H__ */