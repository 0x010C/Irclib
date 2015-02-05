#include "header.h"

unit_data_t *unit_init(const char *hostname)
{
	unit_data_t *data = NULL;
	data = (unit_data_t*) malloc(sizeof(unit_data_t));

	data->sock = irc_init(hostname);
	data->mutex = PTHREAD_MUTEX_INITIALIZER;
	data->nbInbox = 0;
	data->nbOutbox = 0;
	data->inbox = NULL;
	data->outbox = NULL;
	data->hostname = (char*) malloc(sizeof(char)*(strlen(hostname)+1));
	strcpy(data->hostname, hostname, strlen(hostname)+1);
	data->nbChan = 0;
	data->chan = NULL;
	data->defaultCallback = NULL;
	data->callback = NULL;
	data->nbCallback = 0;
	data->callback = NULL;
	data->autoPing = true;
	data->autoPong = true;
	data->autoRelogin = true;
	data->autoJoinChanel = true;
	data->quit = false;

	pthread_create(&data->thloop, NULL, unit_loop, data);

	return data;
}

void *unit_loop(void *data)
{

}

void *unit_rcv(void *data)
{

}

int unit_close(unit_data_t *data)
{
	unsigned long i;

	data->quit = true;
	irc_close(data->sock);
	pthread_join(data->thrcv);
	pthread_join(data->thloop);

	if(data->inbox != NULL)
	{
		for(i=0; i<data->nbInbox; i++)
			free(data->inbox[i]);
		free(data->inbox);
	}
	if(data->outbox != NULL)
	{
		for(i=0; i<data->nbOutbox; i++)
			free(data->outbox[i]);
		free(data->outbox);
	}
	free(data->hostname);
	if(data->chan != NULL)
	{
		for(i=0; i<data->nbChan; i++)
			free(data->chan[i]);
		free(data->chan);
	}
	if(data->callback != NULL)
	{
		for(i=0; i<nbCallback; i++)
			free(data->callback->cmd);
	}

	free(data);
}
