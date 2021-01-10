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

status_t
comms__send_command(int server_sock, commands__command_type_t command_type, const char *payload) {
	status_t retval = UNINITIALIZED;
	comms__protocol_t message = {0};
	ssize_t sent_bytes = 0;
	ssize_t total_sent_bytes = 0;

	message.header.magic = htonl(COMMS_MESSAGE_MAGIC);
	message.body.token = htonl(1);
	message.body.command_type = htonl(command_type);
	strncpy(message.body.payload, payload, sizeof(message.body.payload));

	while(total_sent_bytes != sizeof(message)) {
	    sent_bytes = send(server_sock, (char *)(&message+total_sent_bytes), sizeof(message), 0);
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
