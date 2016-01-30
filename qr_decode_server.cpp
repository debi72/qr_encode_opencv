#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include "main_qr_server.h"


int __cdecl main(void)
{
	
	if (InitConnection() == 1) return 1;
	// Receive until the peer shuts down the connection
	do {

		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);
			printf("Text: %s\n", recvbuf);

			//function with recieved text
			char* output = (char*)malloc(512*sizeof(char*));
			output = Decode(recvbuf);

			// Echo the buffer back to the client
			iSendResult = send(ClientSocket, output, strlen(output) , 0);
			if (iSendResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
			printf("Bytes sent: %d\n", iSendResult);
		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			system("PAUSE");
			return 1;
		}

	} while (iResult > 0);


	closesocket(ClientSocket);
	WSACleanup();
	system("PAUSE");
	return 0;
}


char* Decode(char* image)
{
	TessBaseAPI api;
	api.Init("","eng", OEM_DEFAULT);
	api.SetPageSegMode(static_cast<PageSegMode>(7));
    api.SetOutputName("out");
	printf("File name: %s\n",image);
    PIX   *pixs = pixRead(image);
	STRING text_out;
	api.ProcessPages(image, NULL, 0, &text_out);
	char* res_text = (char*)text_out.string();
	
	int count = 0;
	for(int i = 0; i < strlen(res_text); ++i)
		if (strchr(alph,res_text[i]) != NULL)
			count++;
	
	if (count == 0) {
		printf("NO TEXT IN PICTURE");
		return "NULL";
	}

	char* decode_text = (char*)calloc(DEFAULT_BUFLEN, sizeof(char*));
	int ind = 0;
	for(int i = 0; i < strlen(res_text); ++i)
		if (strchr(alph,res_text[i]) != NULL)
		{
			decode_text[ind] = res_text[i];
			ind++;
		}
	
	printf("RECOGNIZED TEXT: %s\nLength of TEXT: %d\n", decode_text, count);
	

	char* filename = concat("QRText_", decode_text);
	QRErrors res = QREncode(decode_text, filename);

	switch (res){
		case QR_OK:
			printf("QR code has been created\n");
			break;
		case QR_OUT_OF_MEMORY:
			printf("Out of memory error\n");
			break;
		case QR_NULL_POINTER:
			printf("Null pointer exception\n");
			break;
		case QR_UNABLE_OPEN_FILE:
			printf("Unable to open/save file\n");
			break;
	}

	if (res != QR_OK)
	{
		getchar();
		return 0;
	}
	char* path = "D:\\QRGenerator\\QRGenerator\\QRGenerator\\";
	char* file = concat(path, concat(filename, BMP_EXT));
	printf("%s\n", file);
	return file;
}




QRErrors QREncode(char* encodeString, char* out_file)
{
	char outfile[512];
	char* bmp = ".bmp";
	strcpy(outfile, out_file);
	strcat(outfile, bmp);
	unsigned int	unWidth, x, y, l, n, unWidthAdjusted, unDataBytes;
	unsigned char*	pRGBData, *pSourceData, *pDestData;
	QRcode*			pQRC;
	FILE*			f;

	if (pQRC = QRcode_encodeString(encodeString, 0, QR_ECLEVEL_H, QR_MODE_8, 1))
	{
		unWidth = pQRC->width;
		unWidthAdjusted = unWidth * OUT_FILE_PIXEL_PRESCALER * 3;
		if (unWidthAdjusted % 4)
			unWidthAdjusted = (unWidthAdjusted / 4 + 1) * 4;
		unDataBytes = unWidthAdjusted * unWidth * OUT_FILE_PIXEL_PRESCALER;

		// Allocate pixels buffer

		if (!(pRGBData = (unsigned char*)malloc(unDataBytes)))
			return QR_OUT_OF_MEMORY;

		memset(pRGBData, 0xff, unDataBytes);

		BMPFILEHEADER kFileHeader;
		kFileHeader.bfType = 0x4d42;  
		kFileHeader.bfSize = sizeof(BMPFILEHEADER) +	sizeof(BMPINFOHEADER) +	unDataBytes;
		kFileHeader.bfReserved1 = 0;
		kFileHeader.bfReserved2 = 0;
		kFileHeader.bfOffBits = sizeof(BMPFILEHEADER) + sizeof(BMPINFOHEADER);

		BMPINFOHEADER kInfoHeader;
		kInfoHeader.biSize = sizeof(BMPINFOHEADER);
		kInfoHeader.biWidth = unWidth * OUT_FILE_PIXEL_PRESCALER;
		kInfoHeader.biHeight = -((int)unWidth * OUT_FILE_PIXEL_PRESCALER);
		kInfoHeader.biPlanes = 1;
		kInfoHeader.biBitCount = 24;
		kInfoHeader.biCompression = BI_RGB;
		kInfoHeader.biSizeImage = 0;
		kInfoHeader.biXPelsPerMeter = 0;
		kInfoHeader.biYPelsPerMeter = 0;
		kInfoHeader.biClrUsed = 0;
		kInfoHeader.biClrImportant = 0;


		pSourceData = pQRC->data;
		for (y = 0; y < unWidth; y++)
		{
			pDestData = pRGBData + unWidthAdjusted * y * OUT_FILE_PIXEL_PRESCALER;
			for (x = 0; x < unWidth; x++)
			{
				if (*pSourceData & 1)
				{
					for (l = 0; l < OUT_FILE_PIXEL_PRESCALER; l++)
					{
						for (n = 0; n < OUT_FILE_PIXEL_PRESCALER; n++)
						{
							*(pDestData + n * 3 + unWidthAdjusted * l) = PIXEL_COLOR_B;
							*(pDestData + 1 + n * 3 + unWidthAdjusted * l) = PIXEL_COLOR_G;
							*(pDestData + 2 + n * 3 + unWidthAdjusted * l) = PIXEL_COLOR_R;
						}
					}
				}
				pDestData += 3 * OUT_FILE_PIXEL_PRESCALER;
				pSourceData++;
			}
		}


		if (!(fopen_s(&f, outfile, "wb")))
		{
			fwrite(&kFileHeader, sizeof(BMPFILEHEADER), 1, f);
			fwrite(&kInfoHeader, sizeof(BMPINFOHEADER), 1, f);
			fwrite(pRGBData, sizeof(unsigned char), unDataBytes, f);

			fclose(f);
		}
		else
			return QR_UNABLE_OPEN_FILE;

		free(pRGBData);
		QRcode_free(pQRC);
	}
	else
		return QR_NULL_POINTER;

	return QR_OK;
}

int InitConnection()
{
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData); 
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		system("PAUSE");
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result); 
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		system("PAUSE");
		return 1;
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		system("PAUSE");
		return 1;
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		system("PAUSE");
		return 1;
	}

	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);  
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		system("PAUSE");
		return 1;
	}

	// Accept a client socket
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		system("PAUSE");
		return 1;
	}

	// No longer need server socket
	closesocket(ListenSocket);
	return 0;
}


int CloseConnection()
{
	// shutdown the connection since we're done
	iResult = shutdown(ClientSocket, SD_SEND);

	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		system("PAUSE");
		return 1;
	}
	return 0;
}

