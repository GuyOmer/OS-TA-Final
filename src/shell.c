#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <status.h>
#include <common.h>
#include <comms.h>

#define COMMAND_BUFFER_SIZE (256)

status_t
shell__start_shell(char *command) {
	status_t retval = UNINITIALIZED;
	ssize_t command_length = -1;
	char buffer[COMMAND_BUFFER_SIZE] = {0};
	bool should_continue_shell = true;

	// getline requires a variable
	size_t command_buffer_size = COMMAND_BUFFER_SIZE;

	while(should_continue_shell) {
		command_length = getline((char **)&buffer, &command_buffer_size, stdin);
		if (-1 == command_length) {
			retval = SHELL__FAILED_READING_LINE;
			goto cleanup;
		}

		comms__execute_command(command, command_length, &should_continue_shell);
	}

	retval = SUCCESS;
cleanup:
	return retval;
}
