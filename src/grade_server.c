#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <comms.h>
#include <db.h>

int
main(int argc, char *argv[]) {
    status_t retval = UNINITIALIZED;
    db__db_t assitants_db = {};
    db__db_t students_db = {};

    if (argc != 2) {
        fprintf(stderr, "Usage: %s PORT\n", argv[0]);
        retval = INVALID_PARAMETERS;
        goto cleanup;
    }
    retval = db__read_db("./assitants.txt", &assitants_db);
    if(SUCCESS != retval) {
        goto cleanup;
    }

    retval = db__read_db("./students.txt", &students_db);
    if(SUCCESS != retval) {
        goto cleanup;
    }


cleanup:
    db__free_db(&assitants_db);
    db__free_db(&students_db);

    return retval;
} 