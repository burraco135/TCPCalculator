/*
 ============================================================================
 Name        : ClientTCP.c
 Author      : Tommaso Perniola
 Version     : 1.0
 Copyright   : Your copyright notice
 Description : TCP calculator (client)
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "TCP_utils.h"

int main(int argc, char *argv[]) {
	// just for output stream
	setvbuf(stdout, NULL, _IONBF, 0);

	// values taken from program arguments
	int server_port = 0;
	char *server_ip = "";

	// checking/setting default values
	if (argc > 2) {
		if (server_port == DEFAULT_PORT) {
			server_port = atoi(argv[2]);
		}
		if (server_ip == DEFAULT_IP) {
			strcpy(server_ip, argv[1]);
		}
	} else {
		server_port = DEFAULT_PORT;
		strcpy(server_ip, DEFAULT_IP);
	}

#if defined WIN32
	WSADATA wsa_data;
	int result = WSAStartup(MAKEWORD(2,2), &wsa_data);

	if (result != 0)
	{
		errorhandler("Error at WSAStartup()\n");
		return 0;
	}
#endif

	// client's socket
	int client_socket;
	client_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (client_socket == INVALID_SOCKET)
	{
		errorhandler("Socket creation failed.\n");
		clearwinsock();
		return -1;
	}

	// default port and IP taken from TCP_utils library
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(DEFAULT_IP);
	server_addr.sin_port = htons(DEFAULT_PORT);

	// connecting to the server
	if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
		errorhandler("Failed to connect.\n");
		closesocket(client_socket);
		clearwinsock();
		return -1;
	}

	// buffer for sending/receiving data
	char buffer[BUFFERSIZE];

	// sending data to the server
	while(1) {
		// initializing buffer
		memset(&buffer, 0, sizeof(buffer));

		printf("Write a sign followed by two numbers: ");
		scanf(" %[^\n]", buffer);

		if (buffer[0] != '=') {
			int send_input = send(client_socket, buffer, strlen(buffer), 0);

			if (send_input != strlen(buffer)) {
				errorhandler("send() sent a different number of bytes than expected");
				closesocket(client_socket);
				clearwinsock();
				return -1;
			}

			int total_bytes_rcvd = 0, bytes_rcvd;

			printf("Result: ");
			while(total_bytes_rcvd < bytes_rcvd) {
				bytes_rcvd = recv(client_socket, buffer, BUFFERSIZE - 1, 0);

				if (bytes_rcvd <= 0) {
					errorhandler("recv() failed or connection closed prematurely");
					closesocket(client_socket);
					clearwinsock();
					return -1;
				}

				total_bytes_rcvd += bytes_rcvd;
				buffer[bytes_rcvd] = '\0';
				printf("%s\n\n", buffer);
			}
		} else {
			// if user enters escape char, end process
			int send_input = send(client_socket, buffer, strlen(buffer), 0);

			if (send_input != strlen(buffer)) {
				errorhandler("send() sent a different number of bytes than expected");
				closesocket(client_socket);
				clearwinsock();
				return -1;
			}

			printf("\nTask finished");
			closesocket(client_socket);
			clearwinsock();
			printf("\n");
			return(0);
		}

	}
}
