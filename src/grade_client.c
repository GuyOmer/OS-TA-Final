#include <stdio.h>
#include <stdlib.h>

#include <status.h>
#include <comms.h>
#include <shell.h>

int
main(int argc, char *argv[]) {
	status_t retval = UNINITIALIZED;
	int server_sock = -1;
	char command[1024] = {0};

	if (argc != 3) {
		fprintf(stderr, "Usage: %s server port\n", argv[0]);
		retval = MISSING_PARAMETERS;
		goto cleanup;
	}

	// connect to server
	retval = comms__connect(argv[1], atoi((argv[2])), &server_sock);
	if(SUCCESS != retval) {
		goto cleanup;
	}

	shell__start_shell(command);


cleanup:
	return retval;
}
