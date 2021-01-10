#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <status.h>
#include <common.h>
#include <comms.h>
#include <string.h>

status_t
shell__get_input(char *command, int command_buffer_size) {
	status_t retval = UNINITIALIZED;
	ssize_t command_length = -1;
	char *buffer = NULL;
	size_t inner_command_buffer_size = 0;

	buffer = (char *)malloc(command_buffer_size);
	if(NULL == buffer) {
	    retval = SHELL__FAILED_ALLOCATING_BUFFER_FOR_COMMAND;
        goto cleanup;
	}

    (void)printf("> ");
    fflush(stdout);

    if (NULL == fgets(buffer, command_buffer_size-1, stdin)) {
        retval = SHELL__FAILED_READING_LINE;
        goto cleanup;
    }

    (void)strncpy(command, buffer, command_buffer_size);

    retval = SUCCESS;
cleanup:
    SAFE_FREE(buffer);
	return retval;
}

status_t
shell__parse_command(char *command, commands__command_type_t *command_type, size_t *payload_index) {
    size_t request_length = -1;
    size_t actual_command_size = -1;
    size_t i = -1;
    status_t retval = UNINITIALIZED;
    char *request = NULL;
    commands__command_type_t internal_command_type = COMMANDS__MESSAGE_TYPE_INVALID;

    if (NULL == command || command_type == NULL || payload_index == NULL) {
        retval = INVALID_PARAMETERS;
        goto cleanup;
    }

    // assume input is REQUEST <PAYLOAD> - leading space will break this
//    request_length = command - index(command, ' ');
    request = strtok(command, " \n");
    if (NULL == request) {
        retval = SHELL__INPUT_TOO_LONG;
        goto cleanup;
    }
    request_length = strnlen(request, COMMANDS__MAX_COMMAND_LENGTH);

    for(i=0; i<ARRAY_LENGTH(commands__commands_mapping); ++i) {
        // compare both string and lengths to avoid substring matching
        if(
            0 == strncmp(request, commands__commands_mapping[i].command, request_length) &&\
			strnlen(
			    commands__commands_mapping[i].command,
			    STRING_LENGTH(commands__commands_mapping[0].command)
            ) == request_length
        ) {
            internal_command_type = commands__commands_mapping[i].command_type;
            break;
        }
    }

    *command_type = internal_command_type;
    *payload_index = request_length+1;
    retval = SUCCESS;

cleanup:
    return retval;
}