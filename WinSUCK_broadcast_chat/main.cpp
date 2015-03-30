#include <stdlib.h>
#include <winsock.h>
#include <iostream>
#include <string>
#include <conio.h>
#include <process.h>

using namespace std;
#pragma comment(lib,"ws2_32.lib")


void __cdecl recvdProc(void * Args);
void sendProc(sockaddr_in);

SOCKET mainSocket;
bool programExit = true;
bool enableSocketBrodcast = true;

int main(int argc, char *argv[])
{
	WSADATA wsaDATA;
	
	if (WSAStartup(MAKEWORD(1, 1), &wsaDATA) == NO_ERROR)
	{		
		if ((mainSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET) cout << "#2 Creating socket error: " << WSAGetLastError() << endl;
		else
		{		
			setsockopt(mainSocket, SOL_SOCKET, SO_BROADCAST, (char*)&enableSocketBrodcast, sizeof(bool));

			sockaddr_in Sender_addr;
			Sender_addr.sin_family = AF_INET;
			Sender_addr.sin_port = htons(12345);
			Sender_addr.sin_addr.s_addr = inet_addr("255.255.255.255");

			sockaddr_in Recv_addr;
			Recv_addr.sin_family = AF_INET;
			Recv_addr.sin_port = htons(12345);
			Recv_addr.sin_addr.s_addr = INADDR_ANY;


			if (bind(mainSocket, reinterpret_cast<sockaddr*>(&Recv_addr), sizeof(Recv_addr))==0)
			{
				_beginthread(recvdProc, 0, NULL);
				
				do
				{					
					sendProc(Sender_addr);
				} while (programExit);
			}
			else cout << "#3 bind returned error: " << WSAGetLastError() << endl;
		}
		closesocket(mainSocket);	
	}
	else cout << "#1 WSAStartup returned  error: " << WSAGetLastError() << endl;

	WSACleanup();
	_getch();	
	return 0;
}

void __cdecl recvdProc(void * Args)
{
	
	int rozm = 0;
	do
	{	
		char buff[64];
		rozm = recv(mainSocket, reinterpret_cast<char*>(&buff), sizeof(buff), 0);
		if (rozm < 0)
			printf("4 Error: %d\n", WSAGetLastError());
		else
		{
			buff[rozm] = 0;
			printf("%s\n", buff);
		}		
	} while (programExit);
	_endthread();
}

void sendProc(sockaddr_in Sender_addr)
{
	char  wiadomosc[64];
	gethostname(wiadomosc, 64);
	strcat_s(wiadomosc, " connect");
	if (sendto(mainSocket, wiadomosc, strlen(wiadomosc), 0, reinterpret_cast<sockaddr*>(&Sender_addr), sizeof(Sender_addr)) < 0)
		printf("send error: %d", WSAGetLastError());

	do
	{
		std::cin >> wiadomosc;
		if (strcmp(wiadomosc, "exit") ==0)break;

		if (sendto(mainSocket, wiadomosc, strlen(wiadomosc), 0, reinterpret_cast<sockaddr*>(&Sender_addr), sizeof(Sender_addr)) < 0)
			printf("send error: %d", WSAGetLastError());

	} while (true);

	gethostname(wiadomosc, 64);
	strcat_s(wiadomosc, " disconnect");
	if (sendto(mainSocket, wiadomosc, strlen(wiadomosc), 0, reinterpret_cast<sockaddr*>(&Sender_addr), sizeof(Sender_addr)) < 0)
		printf("send error: %d", WSAGetLastError());

	programExit = false;
}