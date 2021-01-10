cmake_minimum_required(VERSION 3.7)
project(untitled C)

set(CMAKE_C_STANDARD 99)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include)

FILE(GLOB SRCS src/*)
set(SERVER_SRCS src/grade_server.c)

CONFIGURE_FILE("assitants.txt" "assitants.txt" COPYONLY)
CONFIGURE_FILE("students.txt" "students.txt" COPYONLY)

ADD_EXECUTABLE(GradeClient src/grade_client.c src/comms.c src/shell.c)
ADD_EXECUTABLE(GradeServer src/grade_server.c src/db.c)
