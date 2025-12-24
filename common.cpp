#include "common.h"
#include <time.h>
#include <string.h>
#include <cstdio>

void init_message(message_t *msg, message_type_t type, uint32_t nm)
{	memset(msg, 0, sizeof(message_t));
	msg->type=type;
	msg->numb=nm;
	msg->timestamp=(uint64_t)time(NULL);
	snprintf(msg->data, sizeof(msg->data), "Message #%u", nm);
}

int validate_message(const message_t *msg)
{	if(msg==NULL) return 0;
	if(msg->type!=msg_PING && msg->type!=msg_PONG) {return 0;}
	uint64_t current_time=(uint64_t)time(NULL);
	if(msg->timestamp > current_time ||
		current_time - msg->timestamp > 3600) {return 0;}
	return 1;
}
