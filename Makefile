SRCS:=src/grade_client.c src/shell.c src/comms.c

GRADE_CLIENT_FILES:=

all: GradeClient GradeServer

GradeClient: ${SRCS}
	gcc	-Werror -Wall -pedantic -O0 -Iinclude -o GradeClient ${SRCS}

GradeServer:

