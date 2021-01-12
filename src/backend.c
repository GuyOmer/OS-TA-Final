#include <stdio.h>
#include <stddef.h>

#include <status.h>
#include <comms.h>
#include <unistd.h>
#include <memory.h>


void
backend__interact_with_client(int client) {
    comms__protocol_t message = {0};
    bool should_terminate = false;

    while(!should_terminate) {
        // if something bad happened, terminate the session
        if(SUCCESS != comms__receive_message(client, &message)) {
            should_terminate = true;
        }

        // if exit command, terminate the session
        if(message.body.command_type == COMMANDS__MESSAGE_TYPE_EXIT) {
            should_terminate = true;
        }
        else {
            //handle_command(message);
            (void)printf("Recieved command: %d, payload: %s\n", message.body.command_type, message.body.payload);
        }
    }
}