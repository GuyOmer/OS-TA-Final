#pragma once

typedef enum status_e {
	SUCCESS = 0,
	UNINITIALIZED,
	MISSING_PARAMETERS,
	
	// shell status
	SHELL__FAILED_ALLOCATING_BUFFER_FOR_COMMAND,
	SHELL__FAILED_READING_LINE,

	//comms status
	COMMS__INVALID_COMMAND,
} status_t;
