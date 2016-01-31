#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN


#include <highgui\highgui.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <video\tracking.hpp>
#include <video\video.hpp>
#include <imgproc\imgproc.hpp>
#include "socket_client.h"
using namespace std;
using namespace cv;

#define NORMAL_WINDOW "Main window"
#define BINARY_WINDOW "Binary window"
#define QR_WINDOW "QR Code"
#define PICTURE_NUM 500

String decode(Mat);

int main()
{
	int count = 0;
	printf("Starting...\n");
	if (InitConnection() == 1) return 1;
	if (ConnectAttempt() == 1) return 1;
	VideoCapture cap(0); //initializing video cam capture with id = 0

	if (!cap.isOpened()) 
	{
		cout << "error: cannot open stream between webcam_0 and application" << endl;
		waitKey(0);
		return -1;
	}
	
	Mat frame;  //creating main image from video cam                          
	namedWindow(NORMAL_WINDOW, CV_WINDOW_AUTOSIZE);  //creating window
	moveWindow(NORMAL_WINDOW, 300, 90);
	while(1)
	{
		count = (count + 1) % PICTURE_NUM;
		if (!cap.read(frame))//trying to read frame form video cam
		{
			cout << "cannot get frame from webcam_0, check drivers" << endl;
			waitKey(0);
			return -1;
		}
		imshow(NORMAL_WINDOW, frame); //show captured
		Mat grayscale(frame.size(), CV_8U);
		cvtColor(frame, grayscale, CV_BGR2GRAY);
		Mat binaryMat(grayscale.size(), grayscale.type());
		threshold(grayscale, binaryMat, 100, 255, THRESH_BINARY);
		namedWindow(BINARY_WINDOW, WINDOW_AUTOSIZE);
		moveWindow(BINARY_WINDOW, 300, 100);
		imshow(BINARY_WINDOW, binaryMat);   //showing binary image
		if (count == 0)
		{
			imwrite("binarywebcam.jpg", binaryMat);
			char* send = concat("D:\\opencv_test\\opencv_test\\","binarywebcam.jpg");
			int res = SendText(send);
			if (res == -1) return -1;
			printf("Sent bytes: %d;\nSent Text: %s\n", res, send);
			if (RecieveText() != 0) return 1;
			printf("Recieve text: %s\n\n", Recieve_Text);
			if (Recieve_Text == "NULL" || Recieve_Text == "NONE")
			{	}
			else {
				Mat qr = imread(Recieve_Text);
				namedWindow(QR_WINDOW, WINDOW_AUTOSIZE);
				imshow(QR_WINDOW, qr);
			}
		}

		if (waitKey(10) == 27)                   //check if ESC is pressed
		{
			cout << "bye!" << endl;
			break;
		}
	}
	if (CloseConnection() == 1) return 1;
	CleanUp();
	frame.release(); //free memory of frame
	destroyAllWindows();  //destroying all windows
	return 0;
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
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(DEFAULT_IP, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		system("PAUSE");
		return 1;
	}

	return 0;
}

int ConnectAttempt() {
	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			system("PAUSE");
			return 1;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		system("PAUSE");
		return 1;
	}
	return 0;
}

int SendText(char* text)
{
	// Send an initial buffer
	iResult = send(ConnectSocket, text, (int)strlen(text), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		system("PAUSE");
		return -1;
	}

	return iResult;
}

int RecieveText()
{
	char* result;
	int recvbuflen = DEFAULT_BUFLEN;
	char* recvbuf = (char*)calloc(512, sizeof(char));

	iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
	if (iResult > 0)
		printf("Bytes received: %d\n", iResult);
	else if (iResult == 0) {
		printf("Connection closed\n");
		return 0;
	}
	else {
		printf("recv failed with error: %d\n", WSAGetLastError());
		return -1;
	}
	Recieve_Text_Length = iResult;
	Recieve_Text = recvbuf;
	memcpy(Recieve_Text, recvbuf, Recieve_Text_Length);
	return 0;
}

int CloseConnection()
{
	int iResult = shutdown(ConnectSocket, SD_SEND);

	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		system("PAUSE");
		return 1;
	}

	return 0;
}

void CleanUp()
{
	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();
}
