#include "header.h"

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
	sin.sin_port = htons(IRC_PORT);
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
	int n, i;
	char message[IRC_MESSAGE_MAXLEN];

	do
	{
		if((n = recv(*((int*) sock), message, sizeof(message) - 1, 0)) < 0)
		{
			perror("recv()");
			exit(errno);
		}
		message[n] = '\0';
		for(i=0; i<n-1; i++)
			printf("%c", message[i]);
		printf("\n");
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

void irc_close(int sock)
{
	close(sock);
}
