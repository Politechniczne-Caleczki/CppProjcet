#include <stdlib.h>
#include <winsock.h>
#include <iostream>
#include <string>
#include <conio.h>
#include <process.h>

using namespace std;
#pragma comment(lib,"ws2_32.lib")

struct Message
{
	char hostName[64];
	char text[128];
};


void __cdecl recvdProc(void * Args);
void sendProc(sockaddr_in);

SOCKET mainSocket;
bool programExit = true;

Message message;
sockaddr_in Recv_addr;


int main(int argc, char *argv[])
{
	WSADATA wsaDATA;
	
	if (WSAStartup(MAKEWORD(1, 1), &wsaDATA) == NO_ERROR)
	{		
		if ((mainSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET) cout << "#2 Creating socket error: " << WSAGetLastError() << endl;
		else
		{		
			bool enableSocketBrodcast = true;
			setsockopt(mainSocket, SOL_SOCKET, SO_BROADCAST, (char*)&enableSocketBrodcast, sizeof(bool));

			sockaddr_in Sender_addr;
			Sender_addr.sin_family = AF_INET;
			Sender_addr.sin_port = htons(12345);
			Sender_addr.sin_addr.s_addr = inet_addr("255.255.255.255");

			sockaddr_in Recv_addr;
			Recv_addr.sin_family = AF_INET;
			Recv_addr.sin_port = htons(12345);
			Recv_addr.sin_addr.s_addr = INADDR_ANY;

			gethostname(message.hostName, sizeof(message.hostName));

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
	int rozmiar = sizeof(Recv_addr);
	int rozm = -1;
	Message _message;	

	do
	{			
		rozm = recvfrom(mainSocket, reinterpret_cast<char*>(&_message), sizeof(Message), 0, reinterpret_cast<sockaddr*>(&Recv_addr), &rozmiar);
		if (rozm < 0)
			printf("4 Error: %d\n", WSAGetLastError());
		else
		{		
			if (strcmp(_message.hostName, message.hostName) != 0)
				printf("%s: %s\n", _message.hostName, _message.text);
		}		
	} while (programExit);

	_endthread();
}


void sendProc(sockaddr_in Sender_addr)
{
	strcpy_s(message.text, "");
	strcat_s(message.text, message.hostName);
	strcat_s(message.text, " connected");

	if (sendto(mainSocket, reinterpret_cast<char*>(&message), sizeof(Message), 0, reinterpret_cast<sockaddr*>(&Sender_addr), sizeof(Sender_addr)) < 0)
		printf("send error: %d", WSAGetLastError());

	do
	{
		char wiadomosc[192] = "";
		std::cin >> message.text;

		if (strcmp(message.text, "exit") ==0)break;		

		if (sendto(mainSocket, reinterpret_cast<char*>(&message), sizeof(Message), 0, reinterpret_cast<sockaddr*>(&Sender_addr), sizeof(Sender_addr)) < 0)
			printf("send error: %d", WSAGetLastError());

	} while (true);

	strcpy_s(message.text, "");
	strcat_s(message.text, message.hostName);
	strcat_s(message.text, " disconnected");

	if (sendto(mainSocket, reinterpret_cast<char*>(&message), sizeof(Message), 0, reinterpret_cast<sockaddr*>(&Sender_addr), sizeof(Sender_addr)) < 0)
		printf("send error: %d", WSAGetLastError());

	programExit = false;
}