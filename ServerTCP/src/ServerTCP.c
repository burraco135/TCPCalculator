/*
 ============================================================================
 Name        : ServerTCP.c
 Author      : Ester Molinari
 Version     : 1.0
 Copyright   : Your copyright notice
 Description : TCP calculator (server)
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "TCP_utils.h"

int main(void) {
	// just for output stream
	setvbuf(stdout, NULL, _IONBF, 0);

	// default port and ID from TCP_utils
#if defined WIN32
	WSADATA wsa_data;
	int result = WSAStartup(MAKEWORD(2,2), &wsa_data);

	if (result != 0)
	{
		errorhandler("Error at WSAStartup()\n");
		return 0;
	}
#endif

	// server's socket
	int server_socket;
	server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_socket == INVALID_SOCKET)
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

	int bind_value = bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if (bind_value < 0)
	{
		errorhandler("bind() failed.\n");

		closesocket(server_socket);
		clearwinsock();
		return -1;
	}

	// start to listen clients requests
	if (listen(server_socket, QLEN) < 0)
	{
		errorhandler("listen() failed.\n");
		closesocket(server_socket);
		clearwinsock();
		return -1;
	}

	struct sockaddr_in client_addr;
	int client_socket;
	int client_len;


	// server's corpus
	while (1)
	{
		printf("Waiting for a client to connect...");

		client_len = sizeof(client_addr);
		if ((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len)) < 0)
		{
			errorhandler("accept() failed.\n");
			closesocket(client_socket);
			clearwinsock();
			return 0;
		}

		printf("\n\nConnection estabilished with %s:%d\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port);

		char buffer[BUFFERSIZE], sign[0]; 	// input/output buffer and sign
		int first_val = 0, second_val = 0;	// operation's input values
		int zero_val = 0;
		int res = 0;						// operation's result

		// do this until client sends the escape sign
		do {
			// initializing buffer and sign
			memset(sign, sizeof(sign), zero_val);
			memset(buffer, sizeof(buffer), zero_val);

			int recv_val = recv(client_socket, buffer, BUFFERSIZE - 1, 0);

			if (recv_val <= 0) {
				errorhandler("recv() failed or connection closed prematurely");
				// closesocket(client_socket);
				// clearwinsock();
				break;
			}

			buffer[recv_val] = '\0';

			// applying input tokenizer
			sscanf(buffer, "%s %d %d", sign, &first_val, &second_val);
			printf("\n >>> Input info <<< ");

			if (sign[0] != '=') {
				printf("\nSign found: %s"
					"\nValues found: %d, %d", sign, first_val, second_val);
			} else {
				printf("\nSign found: %s", sign);
			}


			sign[0] = buffer[0];

			// calling math operations according to sign value
			switch (sign[0]) {
			case '+':
				res = add(first_val, second_val);
				printf("\nSending this result: %d", res);
				break;
			case '-':
				res = sub(first_val, second_val);
				printf("\nSending this result: %d", res);
				break;
			case '/':
				res = division(first_val, second_val);
				printf("\nSending this result: %d", res);
				break;
			case '*':
				res = mult(first_val, second_val);
				printf("\nSending this result: %d", res);
				break;
			case '=':
				printf("\nClosing the connection\n");
				break;
			}

			printf("\n");

			// sending the result to the client
			memset(buffer, sizeof(buffer), zero_val);
			itoa(res, buffer, 10);

			int send_val = send(client_socket, buffer, strlen(buffer), 0);

			if (send_val != strlen(buffer)) {
				errorhandler("send() sent a different number of bytes than expected");
				closesocket(client_socket);
				clearwinsock();
				return -1;
			}

			// clearing the buffer for next operations
			memset(buffer, sizeof(buffer), zero_val);
		} while (sign[0] != '=');
	}
}
