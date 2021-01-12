#include <stdio.h>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <stdbool.h>

#include <status.h>
#include <commands.h>
#include <comms.h>
#include <common.h>
#include <stdlib.h>

status_t
comms__send_command(int server_sock, commands__command_type_t command_type, const char *payload) {
	status_t retval = UNINITIALIZED;
	comms__protocol_t message = {0};
	ssize_t sent_bytes = 0;
	ssize_t total_sent_bytes = 0;
	size_t total_bytes_to_send = 0;
	size_t payload_length = 0;

	payload_length = strnlen(payload, STRING_LENGTH(message.body.payload));

	// serialize the message
	// body_length = total body size - payload buffer size + actual payload
    message.header.body_length = htons(
            sizeof(message.body) - sizeof(message.body.payload) + (payload_length * sizeof(*message.body.payload))
    );
    message.header.magic = htonl(COMMS_MESSAGE_MAGIC);
	message.body.token = htonl(1);
	message.body.command_type = htonl(command_type);
	strncpy(message.body.payload, payload, payload_length);

	total_bytes_to_send = sizeof(message.header) + message.header.body_length;

	while(total_sent_bytes != total_bytes_to_send) {
	    sent_bytes = send(server_sock, (char *)(&message+total_sent_bytes),total_bytes_to_send-total_sent_bytes, 0);
	    if(-1 == sent_bytes) {
	        retval = COMMS__FAILED_SENDING_MESSAGE;
	        goto cleanup;
	    }

	    total_sent_bytes += sent_bytes;
    }

	retval = SUCCESS;

cleanup:
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
comms__create_server_sock(int *server_sock, uint16_t port) {
    int server_fd = INVALID_FD;
    struct sockaddr_in address = {0};
    status_t retval = UNINITIALIZED;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        retval = COMMS__FAILED_CREATING_SERVER_SOCKET;
        goto cleanup;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        retval = COMMS__FAILED_BINDING_ADDRESS;
        goto cleanup;
    }

    if (listen(server_fd, 3) < 0) {
        retval = COMMS__FAILED_LISTEN;
        goto cleanup;
    }

    *server_sock = server_fd;
    server_fd = INVALID_FD;
    retval = SUCCESS;

cleanup:
    SAFE_CLOSE(server_fd);
    return retval;
}

status_t
comms__listen_for_client(int server_sock, int *new_client_sock) {
    status_t retval = UNINITIALIZED;
    int new_socket = INVALID_FD;
    struct sockaddr_in address = {0};
    size_t addrlen = sizeof(address);

    if(new_client_sock == NULL) {
        retval = INVALID_PARAMETERS;
        goto cleanup;
    }

    new_socket = accept(server_sock, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    if (new_socket < 0) {
        retval = COMMS__FAILED_ACCEPTING_NEW_CLIENT;
        goto cleanup;
    }

    *new_client_sock = new_socket;
    new_socket = INVALID_FD;
    retval = SUCCESS;

cleanup:
    SAFE_CLOSE(new_socket);
    return retval;
}

static
status_t
receive_data(int sock, void *buffer, size_t buffer_size) {
    status_t retval = UNINITIALIZED;
    size_t total_bytes_received = 0;
    size_t current_bytes_received = 0;
    char *inner_buffer = NULL;

    inner_buffer = (char *)malloc(buffer_size);
    if (inner_buffer == NULL) {
        retval = COMMS__FAILED_ALLOCATING_BUFFER_FOR_RECIEVING_MESSAGE;
        goto cleanup;
    }

    while(total_bytes_received != buffer_size) {
        current_bytes_received = recv(sock, inner_buffer+total_bytes_received, buffer_size - total_bytes_received, 0);
        total_bytes_received += current_bytes_received;

        if(current_bytes_received == -1) {
            retval = COMMS__FAILED_RECEIVING_MESSAGE_FROM_CLIENT;
            goto cleanup;
        }
    }

    (void)memcpy(buffer, inner_buffer, buffer_size);
    inner_buffer = NULL;
    retval = SUCCESS;

cleanup:
    SAFE_FREE(inner_buffer);
    return retval;
}

status_t
comms__receive_message(int sock, comms__protocol_t *message) {
    status_t retval = UNINITIALIZED;
    comms__protocol_header_t header = {0};
    comms__protocol_body_t body = {0};

    if(SUCCESS != receive_data(sock, &header, sizeof(header))) {
        goto cleanup;
    }
    header.magic = htonl(header.magic);
    header.body_length = htons(header.body_length);

    if(header.magic != COMMS_MESSAGE_MAGIC) {
        // this is quite severe, however not certain if i should handle this
        retval = COMMS__INCORRECT_MAGIC;
        goto cleanup;
    }

    if(SUCCESS != receive_data(sock, &body, header.body_length)) {
        goto cleanup;
    }

    body.token = htonl(body.token);
    body.command_type = htonl(body.command_type);

    memcpy(&message->header, &header, sizeof(header));
    memcpy(&message->body, &body, sizeof(body));
    retval = SUCCESS;

cleanup:
    return retval;

}
