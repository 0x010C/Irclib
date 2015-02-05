#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <stdarg.h>

#define INVALID_SOCKET				-1
#define SOCKET_ERROR				-1

#define SOH							0x01
#define SP							0x20
#define CR							0x0d
#define LF							0x0a

#define IRC_PORT					6667
#define IRC_MESSAGE_MAXLEN			512

#define IRC_MODE_ADD				0x0000
#define IRC_MODE_RM					0x0001

#define IRC_MODE_CHANOP				0x0002
#define IRC_MODE_PRIVATE			0x0004
#define IRC_MODE_SECRET				0x0008
#define IRC_MODE_INVITATION			0x0010
#define IRC_MODE_TOPIC				0x0020
#define IRC_MODE_NOTFROMOUTSIDE		0x0040
#define IRC_MODE_MODERATE			0x0080
#define IRC_MODE_LIMIT				0x0100
#define IRC_MODE_BAN				0x0200
#define IRC_MODE_VOICE				0x0400
#define IRC_MODE_KEY				0x0800

#define IRC_MODE_INVISIBLE			0x1000
#define IRC_MODE_NOTIFIED			0x2000
#define IRC_MODE_WALLOPS			0x4000
#define IRC_MODE_USEROP				0x8000

#define MIN(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })

typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr sockaddr;
typedef struct in_addr in_addr;

enum bool
{
	false,
	true
};
typedef enum bool bool;

struct unit_callback_t
{
	char *cmd;
	void (*function)(char *);
}
typedef struct unit_callback_t unit_callback_t;

struct unit_data_t
{
	int sock;
	pthread_mutex_t mutex;
	pthread_t thloop;
	pthread_t thrcv;
	unsigned long nbInbox;
	unsigned long nbOutbox;
	char **inbox;
	char **outbox;
	char *hostname;
	unsigned int nbChan;
	char **chan;
	void (*defaultCallback)(char *);
	unsigned int nbCallback;
	unit_callback_t *callback;
	bool autoPing;
	bool autoPong;
	bool autoRelogin;
	bool autoJoinChanel;
	bool quit;
};
typedef struct unit_data_t unit_data_t;

int irc_init(const char *hostname);
void *irc_rcv(void *sock);
void irc_send(int sock, char *message);
void irc_close(int sock);

void cmd_send(int sock, char *commande, ...);
void cmd_nick(int sock, char *pseudo);
void cmd_user(int sock, char *pseudo, char *host, char *serv, char *realName);
void cmd_quit(int sock, char *why);
void cmd_ping(int sock, char *key);
void cmd_pong(int sock, char *key);
void cmd_join(int sock, char *chan);
void cmd_part(int sock, char *chan);
void cmd_usermode(int sock, unsigned int newMode, char *pseudo);
void cmd_chanmode(int sock, char *chan, unsigned int newMode, unsigned long limit, char *extra);
void cmd_oper(int sock, char *user, char *password);
void cmd_topic(int sock, char *chan, char *newTopic);
void cmd_names(int sock, char *chan);
void cmd_list(int sock, char *chan);
void cmd_invite(int sock, char *pseudo, char *chan);
void cmd_kick(int sock, char *chan, char *pseudo, char *why);
void cmd_version(int sock, char *serv);
void cmd_privmsg(int sock, char *chanOrPseudo, char *message);
void cmd_action(int sock, char *chanOrPseudo, char *message);
void cmd_notice(int sock, char *chanOrPseudo, char *message);
void cmd_who(int sock, char *chanOrPseudo);
void cmd_whois(int sock, char *serv, char *pseudo);
void cmd_whowas(int sock, char *pseudo);
void cmd_ison(int sock, char *pseudo);

unit_data_t *unit_init(const char *hostname);
int unit_close();
void *unit_loop(void *data);
void *unit_rcv(void *data);
