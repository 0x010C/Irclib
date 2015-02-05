#include "header.h"

void cmd_send(int sock, char *commande, ...)
{
	va_list parametres;
	char *message, *p, *next;
	message = malloc(sizeof(char)*(strlen(commande)+1));
	sprintf(message, "%s", commande);

	va_start(parametres, commande);
	next = (char*) va_arg(parametres, char*);
	while(next != NULL)
	{
		p = next;
		next = (char*) va_arg(parametres, char*);
		if(next != NULL)
		{
			message = realloc(message, sizeof(char)*(strlen(message)+strlen(p)+2));
			sprintf(message, "%s%c%s", message, SP, p);
		}
		else
		{
			message = realloc(message, sizeof(char)*(strlen(message)+strlen(p)+3));
			sprintf(message, "%s%c:%s", message, SP, p);
		}
	}
	va_end(parametres);
	irc_send(sock, message);
	free(message);
}

void cmd_nick(int sock, char *pseudo)
{
	cmd_send(sock, "NICK", pseudo, NULL);
}

void cmd_user(int sock, char *pseudo, char *host, char *serv, char *realName)
{
	if(host == NULL)
		host = pseudo;
	if(realName == NULL)
		realName = pseudo;
	cmd_send(sock, "USER", pseudo, host, serv, realName, NULL);
}

void cmd_quit(int sock, char *why)
{
	cmd_send(sock, "QUIT", why, NULL);
}

void cmd_ping(int sock, char *key)
{
	cmd_send(sock, "PING", key, NULL);
}

void cmd_pong(int sock, char *key)
{
	cmd_send(sock, "PONG", key, NULL);
}

void cmd_join(int sock, char *chan)
{
	cmd_send(sock, "JOIN", chan, NULL);
}

void cmd_part(int sock, char *chan)
{
	cmd_send(sock, "PART", chan, NULL);
}

void cmd_usermode(int sock, unsigned int newMode, char *pseudo)
{
	int i = 1;
	char param[6];
	
	if((newMode & IRC_MODE_RM) == 0)
		param[0] = '+';
	else
		param[0] = '-';
	
	if((newMode & IRC_MODE_INVISIBLE) != 0)
		param[i++] = 'i';
	if((newMode & IRC_MODE_NOTIFIED) != 0)
		param[i++] = 's';
	if((newMode & IRC_MODE_WALLOPS) != 0)
		param[i++] = 'w';
	if((newMode & IRC_MODE_USEROP) != 0)
		param[i++] = 'o';
	param[i] = '\0';
	
	if(i == 1)
		return;

	cmd_send(sock, "MODE", pseudo, param, NULL);
}

void cmd_chanmode(int sock, char *chan, unsigned int newMode, unsigned long limit, char *extra)
{
	int i = 1;
	char param[10], charLimit[11];
	bool sendExtra = false, sendLimit = false;
	
	if((newMode & IRC_MODE_RM) == 0)
		param[0] = '+';
	else
		param[0] = '-';

	if((newMode & IRC_MODE_PRIVATE) != 0)
		param[i++] = 'p';
	if((newMode & IRC_MODE_SECRET) != 0)
		param[i++] = 's';
	if((newMode & IRC_MODE_INVITATION) != 0)
		param[i++] = 'i';
	if((newMode & IRC_MODE_TOPIC) != 0)
		param[i++] = 't';
	if((newMode & IRC_MODE_NOTFROMOUTSIDE) != 0)
		param[i++] = 'n';
	if((newMode & IRC_MODE_MODERATE) != 0)
		param[i++] = 'm';

	if((newMode & IRC_MODE_CHANOP) != 0 && extra != NULL)
	{
		param[i++] = 'o';
		sendExtra = true;
	}
	else if((newMode & IRC_MODE_BAN) != 0 && extra != NULL)
	{
		param[i++] = 'b';
		sendExtra = true;
	}
	else if((newMode & IRC_MODE_VOICE) != 0 && extra != NULL)
	{
		param[i++] = 'v';
		sendExtra = true;
	}
	else if((newMode & IRC_MODE_KEY) != 0 && extra != NULL)
	{
		param[i++] = 'k';
		sendExtra = true;
	}
	else if((newMode & IRC_MODE_LIMIT) != 0 && limit >= 0)
	{
		param[i++] = 'l';
		sendLimit = true;
		sprintf(charLimit, "%ld", limit);
	}
	param[i] = '\0';

	if(i == 1)
		return;

	else if(sendExtra)
		cmd_send(sock, "MODE", chan, param, extra, NULL);
	else if(sendLimit)
		cmd_send(sock, "MODE", chan, param, charLimit, NULL);
	else
		cmd_send(sock, "MODE", chan, param, NULL);
}

void cmd_oper(int sock, char *user, char *password)
{
	cmd_send(sock, "OPER", user, password, NULL);
}

void cmd_topic(int sock, char *chan, char *newTopic)
{
	cmd_send(sock, "TOPIC", chan, newTopic, NULL);
}

void cmd_names(int sock, char *chan)
{
	cmd_send(sock, "NAMES", chan, NULL);
}

void cmd_list(int sock, char *chan)
{
	cmd_send(sock, "LIST", chan, NULL);
}

void cmd_invite(int sock, char *pseudo, char *chan)
{
	cmd_send(sock, "INVITE", pseudo, chan, NULL);
}

void cmd_kick(int sock, char *chan, char *pseudo, char *why)
{
	cmd_send(sock, "KICK", chan, pseudo, why, NULL);
}

void cmd_version(int sock, char *serv)
{
	cmd_send(sock, "VERSION", serv, NULL);
}

void cmd_privmsg(int sock, char *chanOrPseudo, char *message)
{
	cmd_send(sock, "PRIVMSG", chanOrPseudo, message, NULL);
}

void cmd_action(int sock, char *chanOrPseudo, char *message)
{
	char *actionMessage = (char*) malloc(sizeof(char)*(strlen(message)+10));
	sprintf(actionMessage, "%cACTION%c%s%c", SOH, SP, message, SOH);
	cmd_send(sock, "PRIVMSG", chanOrPseudo, actionMessage, NULL);
}

void cmd_notice(int sock, char *chanOrPseudo, char *message)
{
	cmd_send(sock, "NOTICE", chanOrPseudo, message, NULL);
}

void cmd_who(int sock, char *chanOrPseudo)
{
	cmd_send(sock, "WHO", chanOrPseudo, NULL);
}

void cmd_whois(int sock, char *serv, char *pseudo)
{
	if(serv == NULL)
		cmd_send(sock, "WHOIS", pseudo, NULL);
	else
		cmd_send(sock, "WHOIS", serv, pseudo, NULL);
}

void cmd_whowas(int sock, char *pseudo)
{
	cmd_send(sock, "WHOWAS", pseudo, NULL);
}

void cmd_ison(int sock, char *pseudo)
{
	cmd_send(sock, "ISON", pseudo, NULL);
}
