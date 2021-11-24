/*
 * TCP_utils.h
 *
 *  Created on: 21 nov 2021
 *      Author: Ester Molinari, Tommaso Perniola
 */

#ifndef TCP_UTILS_H_
#define TCP_UTILS_H_

#if defined WIN32
#include <winsock2.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#define QLEN 5 // size of server's request queue
#define BUFFERSIZE 512 // size of message buffer

#define DEFAULT_IP "127.0.0.1" // default server IP
#define DEFAULT_PORT 27050 // default server port

// for error messages
void errorhandler(char *errorMessage)
{
	printf("%s", errorMessage);
	fflush(stdout);
}

// for windows sockets
void clearwinsock()
{
#if defined WIN32
	WSACleanup();
#endif
}

// server's math utils
int add(int a, int b)
{
	return (a+b);
}

int mult(int a, int b)
{
	return (a*b);
}

int sub(int a, int b)
{
	return (a-b);
}

int division(int a, int b)
{
	return (int)(a/b);
}

#endif /* TCP_UTILS_H_ */
