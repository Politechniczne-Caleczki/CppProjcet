#include <stdlib.h>
#include <winsock.h>
#include <iostream>
#include <string>
#include <conio.h>
#include <process.h>

using namespace std;
#pragma comment(lib,"ws2_32.lib")

int odbierz(SOCKET Rsocket, unsigned short* buff, unsigned int maxSize)
{
	if (buff == NULL)return -3;
	int index = 0;

	while (index< maxSize)
	{
		int i = recv(Rsocket, reinterpret_cast<char*>(buff), maxSize - index, 0);
		if (i == 0)return -1;
		else if (i < 0)return -2;
		else
		{
			index += i;
			if (index % 2 == 0)
				if (buff[index / 2] == 0)return index / 2;
		}
	}
	return index / 2;
}


int main(int argc, char *argv[])
{
	WSADATA wsaDATA;

	if (WSAStartup(MAKEWORD(1, 1), &wsaDATA) == NO_ERROR)
	{
		SOCKET TCPsocket, UDPsocket;

		TCPsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		UDPsocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		if (TCPsocket != INVALID_SOCKET || UDPsocket != INVALID_SOCKET)
		{
			sockaddr_in serverAdres;
			serverAdres.sin_family = AF_INET;
			serverAdres.sin_port = htonl(1234);
			serverAdres.sin_addr.s_addr = inet_addr("127.0.0.1");

			u_long val = 1;

			if (bind(TCPsocket, reinterpret_cast<sockaddr*>(&serverAdres), sizeof(serverAdres)) != SOCKET_ERROR)
			{
				if (bind(UDPsocket, reinterpret_cast<sockaddr*>(&serverAdres), sizeof(serverAdres)) != SOCKET_ERROR)
				{
					if (listen(TCPsocket, 1) >= 0)
					{
						ioctlsocket(TCPsocket, FIONBIO, &val);

						sockaddr_in clientAdres;
						clientAdres.sin_family = AF_INET;
						clientAdres.sin_port = htonl(1234);
						clientAdres.sin_addr.s_addr = inet_addr("127.0.0.1");

						SOCKET clientSocket = INVALID_SOCKET;

						int clientSize = sizeof(clientAdres);

						while (true)
						{
							if (_kbhit() && _getch() == ' ')
							{
								char buff[] = "TEST";
								sendto(UDPsocket, buff, sizeof(buff), 0, reinterpret_cast<sockaddr*>(&clientAdres), sizeof(clientAdres));
							}

							if (clientSocket == INVALID_SOCKET)
							{
								fd_set read, write, exc;
								FD_ZERO(&read);
								FD_ZERO(&write);
								FD_ZERO(&exc);

								FD_SET(TCPsocket, &read);
								FD_SET(TCPsocket, &write);
								FD_SET(TCPsocket, &exc);

								timeval timeVal = { 1, 0 };

								if (select(0, &read, &write, &exc, &timeVal) > 0)
								{
									clientSocket = accept(TCPsocket, reinterpret_cast<sockaddr*>(&clientAdres), &clientSize);
									if (clientSocket != INVALID_SOCKET)
									{
										unsigned short buff[512];
										if (odbierz(clientSocket, buff, sizeof(buff)) > 0)
										{
											for (; buff[0] < buff[1]; buff[0] += buff[2])
												send(clientSocket, reinterpret_cast<char*>(buff[0]), sizeof(buff[0]), 0);
										}
										else clientSocket == INVALID_SOCKET;
									}
								}
							}
							else
							{
								unsigned short buff[512];
								if (odbierz(clientSocket, buff, sizeof(buff)) > 0)
								{
									for (; buff[0] < buff[1]; buff[0] += buff[2])
										send(clientSocket, reinterpret_cast<char*>(buff[0]), sizeof(buff[0]), 0);
								}
								else clientSocket == INVALID_SOCKET;
							}
						}
						shutdown(clientSocket, 2);
						closesocket(clientSocket);
						shutdown(TCPsocket, 2);
						closesocket(TCPsocket);
						closesocket(UDPsocket);
					}
					else cout << "4. Listen error" << WSAGetLastError();
				}
				else cout << "3. Bind error" << WSAGetLastError();
			}
			else cout << "3. Bind error" << WSAGetLastError();
		}
		else cout << "2. Create Socket error" << WSAGetLastError();
	}
	else cout << "1. StartUp error" << WSAGetLastError();
	WSACleanup();
	_getch();
	return 0;
}