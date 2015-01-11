#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

#define PORT 6667
#define IRC_MESSAGE_MAXLEN 512

#define MIN(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })

typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr sockaddr;
typedef struct in_addr in_addr;


int irc_init(const char *hostname)
{
	int sock;
	struct hostent *hostinfo = NULL;
	sockaddr_in sin = { 0 };

	if((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		perror("socket()");
		exit(errno);
	}

	if((hostinfo = gethostbyname(hostname)) == NULL)
	{
		fprintf (stderr, "Unknown host %s.\n", hostname);
		exit(EXIT_FAILURE);
	}

	sin.sin_addr = *(in_addr *) hostinfo->h_addr;
	sin.sin_port = htons(PORT);
	sin.sin_family = AF_INET;
	if(connect(sock,(sockaddr *) &sin, sizeof(sockaddr)) == SOCKET_ERROR)
	{
		perror("connect()");
		exit(errno);
	}
	return sock;
}

void *irc_rcv(void *sock)
{
	int n;
	char message[IRC_MESSAGE_MAXLEN];

	do
	{
		if((n = recv(*((int*) sock), message, sizeof(message) - 1, 0)) < 0)
		{
			perror("recv()");
			exit(errno);
		}
		message[n] = '\0';
		printf("\n< In : %s\n", message);
	} while(message[0] != '\0');

	return NULL;
}

void irc_send(int sock, char *message)
{
	int min;
	char *m;

	min = MIN(strlen(message), IRC_MESSAGE_MAXLEN-2);
	m = (char*) malloc(sizeof(char)*(min+3));
	strncpy(m, message, min);
	m[min] = 0x0D;
	m[min+1] = 0x0A;
	m[min+2] = '\n';

	printf("\n> Out : '%s'", m);
	if(send(sock, m, min+2, 0) < 0)
	{
		perror("send()");
		exit(errno);
	}
}

int main(int argc, char **argv)
{
	int sock;
	int n;
	pthread_t threadIrcRcv;
	char buffer[IRC_MESSAGE_MAXLEN-1];

	sock = irc_init("verne.freenode.net");
	if(pthread_create(&threadIrcRcv, NULL, irc_rcv, &sock))
	{ 
		perror("pthread_create irc_rcv"); 
		exit(EXIT_FAILURE); 
	}

	irc_send(sock, "NICK kiwi_0xTest");
	irc_send(sock, "USER 0x010C 0x010C verne.freenode.net :0x010C");
	
	do
	{
		fgets(buffer, IRC_MESSAGE_MAXLEN-2, stdin);
		char *p = strchr(buffer, '\n');
		*p = '\0';
		irc_send(sock, buffer);
	} while(buffer[0] != '\0');
	
	pthread_join(threadIrcRcv, NULL);
	
	printf("Close socket\n");
	close(sock);

	printf("Exit\n");
	return EXIT_SUCCESS;
}
