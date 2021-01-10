#pragma once
#include <status.h>

status_t
shell__get_input(char *command, int command_buffer_size);

status_t
shell__parse_command(char *command, commands__command_type_t *command_type, size_t *payload_index);