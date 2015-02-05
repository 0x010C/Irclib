#include "header.h"

int main(int argc, char **argv)
{
	int sock;
	int n;
	pthread_t threadIrcRcv;
	char buffer[IRC_MESSAGE_MAXLEN-1];

	sock = irc_init("holmes.freenode.net");
	if(pthread_create(&threadIrcRcv, NULL, irc_rcv, &sock))
	{ 
		perror("pthread_create irc_rcv"); 
		exit(EXIT_FAILURE); 
	}

	cmd_nick(sock, "kiwi_0xTest");
	cmd_user(sock, "0x010C", "patate", "freenode.net", "truc");

	sleep(10);

	printf("Exit\n");
	return EXIT_SUCCESS;
}


/*	cmd_quit(sock, " Au revoir les gens !");*/
