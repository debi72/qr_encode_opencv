#ifndef __SOCKET_CLIENT_H__
#define __SOCKET_CLIENT_H__

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "457"
#define DEFAULT_IP "localhost"
SOCKET ConnectSocket = INVALID_SOCKET;
WSADATA wsaData;
struct addrinfo *result = NULL,
	*ptr = NULL,
	hints;

int iResult;
char* Recieve_Text;
int Recieve_Text_Length;

int InitConnection();
int ConnectAttempt();
int SendText(char*);
int RecieveText();
int CloseConnection();
void CleanUp();

char* concat(char* fisrt, char* second)
{
	char* res;
	res = (char*)malloc(strlen(fisrt) + strlen(second) + 1);
	strcpy(res, fisrt);
	strcat(res, second);
	return res;
}


#endif /* __SOCKET_CLIENT_H__ */


