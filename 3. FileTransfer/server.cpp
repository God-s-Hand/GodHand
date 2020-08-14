#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<Windows.h>
#include<process.h>

#define BUF_SIZE 1024
#define MAX_CLNT 256

void error_handling(char* message);

int clntCnt = 0;

int main()
{
	WSADATA wsaData;
	SOCKET hServSock, hClntSock;
	SOCKADDR_IN serv_adr, clnt_adr;

	FILE* img;
	FILE* text;

	char buf[BUF_SIZE];
	int clntAdrSz;
	int read_len;
	int fileNumber = 0;


	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		error_handling("WSAStartup() error!");

	hServSock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&serv_adr, 0, sizeof(serv_adr));

	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi("9200"));

	if (bind(hServSock, (SOCKADDR*)&serv_adr, sizeof(serv_adr)) == SOCKET_ERROR)
		error_handling("bind() error");
	if (listen(hServSock, 5) == SOCKET_ERROR)
		error_handling("listen() error");

	clntAdrSz = sizeof(clnt_adr);
	while (1)
	{
		hClntSock = accept(hServSock, (SOCKADDR*)&clnt_adr, &clntAdrSz);

		char file_name[BUF_SIZE];
		char *ptr = NULL;
		char tmp[10] = { 0 };
		
		memset(buf, 0x00, BUF_SIZE);

		printf("new client connect : %s\n", inet_ntoa(clnt_adr.sin_addr));

		read_len = recv(hClntSock, buf, BUF_SIZE, 0); //우선 파일 네임 받기
		if (read_len > 0)
		{
			fileNumber += 1;
			strcpy(file_name, buf);
			ptr = strtok(file_name, ".");
			strcpy(file_name, ptr);
			_itoa(fileNumber, tmp, 10);
			strcat(file_name, tmp);
			
			strcat(file_name, ".bmp");
			printf("%s > %s\n", inet_ntoa(clnt_adr.sin_addr), file_name);

			text = fopen("fileIndex.php", "r+");
			fseek(text, -1, SEEK_END);
		}

		fwrite(",{\"number\":", 1, strlen(",{\"number\":"), text);
		fwrite(tmp, 1, strlen(tmp), text);
		fwrite(",\"filename\":\"test", 1, strlen(",\"filename\":\"test"), text);
		fwrite(tmp, 1, strlen(tmp), text);
		fwrite(".bmp\"}]", 1, strlen(".bmp\"}]"), text);
		fclose(text);

		img = fopen(file_name, "wb");

		while (1)
		{
			memset(buf, 0x00, BUF_SIZE);
			int file_read_len = recv(hClntSock, buf, BUF_SIZE, 0);
			fwrite(buf, 1, file_read_len, img);
			if (file_read_len == EOF | file_read_len == 0)
			{
				for (int i = 0; i < 600; i++) {
					//wait until transfer clear
				}
				fclose(img);
				printf("finish file \n");
				break;
			}
		}
	}
	closesocket(hServSock);

	WSACleanup();

	system("pause");

	return 0;
}

void error_handling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}