#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

typedef enum {		//тип у сообщений.
	msg_PING=1,
	msg_PONG=2
} message_type_t;

typedef struct{
	message_type_t type;	//пинг или понг.
	uint32_t numb;	//какой по номеру мсж.
	uint64_t timestamp;	//время созд.
	char data[256];		//потом.

}message_t;

void init_message(message_t *msg, message_type_t type, uint32_t nm);
int validate_message(const message_t *msg);

#endif
