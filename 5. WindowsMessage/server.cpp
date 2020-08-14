#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<Windows.h>
#include<process.h>

#define BUF_SIZE 100
#define MAX_CLNT 256

void error_handling(char* message);

int clntCnt = 0;

int main()
{
	WSADATA wsaData;
	SOCKET hServSock, hClntSock;
	SOCKADDR_IN serv_adr, clnt_adr;
	int clntAdrSz;
	int delay = 0;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		error_handling("WSAStartup() error!");
	hServSock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi("9000"));

	if (bind(hServSock, (SOCKADDR*)&serv_adr, sizeof(serv_adr)) == SOCKET_ERROR)
		error_handling("bind() error");
	if (listen(hServSock, 5) == SOCKET_ERROR)
		error_handling("listen() error");
	clntAdrSz = sizeof(clnt_adr);
	while (1) {
		hClntSock = accept(hServSock, (SOCKADDR*)&clnt_adr, &clntAdrSz);
		printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr));
		int str_len = 0;
		char msg[BUF_SIZE];
		while ((str_len = recv(hClntSock, msg, BUF_SIZE, 0)) != 0)
		{
			printf("\n");
			printf("%d\n", str_len);
			msg[str_len] = NULL;
			printf("%c\n", msg[0]);
			if (msg[0] == 'L')
			{
				keybd_event(VK_LEFT, 0, 0, 0);
				keybd_event(VK_LEFT, 0, KEYEVENTF_KEYUP, 0);
			}
			else if (msg[0] == 'R')
			{
				keybd_event(VK_RIGHT, 0, 0, 0);
				keybd_event(VK_RIGHT, 0, KEYEVENTF_KEYUP, 0);
			}
			else if (msg[0] == 'U')
			{
				keybd_event(VK_UP, 0, 0, 0);
				keybd_event(VK_UP, 0, KEYEVENTF_KEYUP, 0);
			}
			else if (msg[0] == 'D')
			{
				keybd_event(VK_DOWN, 0, 0, 0);
				keybd_event(VK_DOWN, 0, KEYEVENTF_KEYUP, 0);
			}
			else if (msg[0] == 'S')
			{
				keybd_event(VK_F5, 0, 0, 0);
				keybd_event(VK_F5, 0, KEYEVENTF_KEYUP, 0);
			}
			else if (msg[0] == 'E')
			{
				keybd_event(VK_ESCAPE, 0, 0, 0);
				keybd_event(VK_ESCAPE, 0, KEYEVENTF_KEYUP, 0);
			}
		}
	}
	
	closesocket(hServSock);
	WSACleanup();
	return 0;
}

void error_handling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}