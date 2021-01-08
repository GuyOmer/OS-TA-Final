#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <stdbool.h>

#include <status.h>
#include <commands.h>
#include <common.h>

static
status_t
send_command(commands__command_type_t command_type, const char *payload, size_t payload_length) {
	status_t retval = UNINITIALIZED;
	comms__protocol_t message = {0};

	message.header.magic = COMMS_MESSAGE_MAGIC;
	message.body.token = 1;
	message.body.message_type = message_type;
	strncpy(message.body.payload, payload, payload_length);

	// serialize the message befor sending
	send(server_sock, &message, sizeof*message), 0


//cleanup:
	return retval;
}


status_t
comms__connect(const char *address, uint16_t port, int *sock) {
	int inner_sock = -1; 
    struct sockaddr_in serv_addr; 

    if ((inner_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(port); 
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, address, &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 
   
    if (connect(inner_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return -1; 
    } 

	*sock = inner_sock;
    return 0; 
}

status_t
comms__execute_command(const char *command, size_t command_length, bool *is_exist_command) {
	status_t retval = UNINITIALIZED;
	commands__command_type_t command_type = COMMANDS__MESSAGE_TYPE_INVALID;
	int actual_command_size = -1;
	size_t i = 0;
	char * payload = NULL;
	size_t payload_length = -1;

	actual_command_size = (int)(index(command, ' ') - command);

	for (i=0; i< ARRAY_LENGTH(commands__commands_mapping); ++i) {
		// compare both string and lengths to avoid substring matching
		if(
			0 == strncmp(command, commands__commands_mapping[i].command, actual_command_size) &&\
			STRING_LENGTH(commands__commands_mapping[i].command) == actual_command_size
		) {
			command_type = commands__commands_mapping[i].command_type;
			break;
		}
	}

	if (command_type == COMMANDS__MESSAGE_TYPE_INVALID) {
		retval = COMMS__INVALID_COMMAND;
		goto cleanup;
	}

	payload = (char *)command + actual_command_size;
	payload_length = command_length - actual_command_size;

	send_command(command_type, payload, payload_length);

	retval = SUCCESS;
cleanup:
	return retval;
}

