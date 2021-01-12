#pragma once
#include <stdint.h>
#include <stdbool.h>

#include <commands.h>
#include <status.h>

#define COMMS_MESSAGE_MAGIC (0x1336)

#pragma pack(1)
typedef struct comms__protocol_header_s {
    uint32_t magic;
    uint16_t body_length;
} comms__protocol_header_t;

typedef struct comms__protocol_body_s {
	uint32_t token;
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

status_t
comms__receive_message(int sock, comms__protocol_t *message);

status_t
comms__create_server_sock(int *server_sock, uint16_t port);

status_t
comms__listen_for_client(int server_sock, int *new_client_sock);