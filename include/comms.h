#pragma once
#include <stdint.h>
#include <stdbool.h>

#include <commands.h>
#include <status.h>

#define COMMS_MESSAGE_MAGIC (0x1336)

#pragma pack(1)
typedef struct comms__protocol_header_s {
	int magic;
	int payload_length;
} comms__protocol_header_t;

typedef struct comms__protocol_body_s {
	int token;
	commands__command_type_t command_type;
	char payload[COMMANDS__MAX_COMMAND_LENGTH];
} comms__protocol_body_t;

typedef struct comms__protocol_s {
	comms__protocol_header_t header;
	comms__protocol_body_t body;
} comms__protocol_t;

#pragma pack()


status_t
comms__connect(const char *address, uint16_t port, int *sock);

status_t
comms__send_command(int server_sock, commands__command_type_t command_type, const char *payload);
