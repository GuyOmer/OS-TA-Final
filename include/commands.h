#pragma once

#define COMMANDS__MAX_COMMAND_LENGTH (256)

typedef enum commands__command_type_e {
	COMMANDS__MESSAGE_TYPE_INVALID,
	COMMANDS__MESSAGE_TYPE_LOGOUT,
	COMMANDS__MESSAGE_TYPE_LOGIN,
	COMMANDS__MESSAGE_TYPE_EXIT,
	COMMANDS__MESSAGE_TYPE_READ_GRADE,
	COMMANDS__MESSAGE_TYPE_GRADE_LIST,
	COMMANDS__MESSAGE_TYPE_UPDATE_GRADE
} commands__command_type_t;

typedef struct commands__command_s {
	char command[COMMANDS__MAX_COMMAND_LENGTH];
	commands__command_type_t command_type;
} commands__command_t;
	
static const commands__command_t commands__commands_mapping[] = {
	{"Logout",      COMMANDS__MESSAGE_TYPE_LOGOUT},
	{"Login",       COMMANDS__MESSAGE_TYPE_LOGIN},
	{"Exit",        COMMANDS__MESSAGE_TYPE_EXIT},
	{"ReadGrade",   COMMANDS__MESSAGE_TYPE_READ_GRADE},
	{"GradeList",   COMMANDS__MESSAGE_TYPE_GRADE_LIST},
	{"UpdateGrade", COMMANDS__MESSAGE_TYPE_UPDATE_GRADE},
};
