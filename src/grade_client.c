#include <stdio.h>
#include <stdlib.h>

#include <status.h>
#include <comms.h>
#include <shell.h>
#include <common.h>

int
main(int argc, char *argv[]) {
    status_t retval = UNINITIALIZED;
    int server_sock = -1;
    char command[COMMANDS__MAX_COMMAND_LENGTH] = {0};
    commands__command_type_t command_type = COMMANDS__MESSAGE_TYPE_INVALID;
    size_t payload_index = 0;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s server port\n", argv[0]);
        retval = INVALID_PARAMETERS;
        goto cleanup;
    }

    // connect to server
    retval = comms__connect(
        argv[1],
        (int) strtol(argv[2], (char **) NULL, 10),
        &server_sock
    );
    if (SUCCESS != retval) {
        goto cleanup;
    }

    while(1) {
        // shell__get_input reserves space for null-terminator
        if(SUCCESS != shell__get_input(command, ARRAY_LENGTH(command))) {
            goto cleanup;
        }

        shell__parse_command(command, &command_type, &payload_index);
        if (command_type == COMMANDS__MESSAGE_TYPE_EXIT) {
            // printf("Bye bye...\n");
            break;
        }

        comms__send_command(server_sock, command_type, command+payload_index);
//        comms__print_response(server_sock);
    }


    cleanup:
    return retval;
}
