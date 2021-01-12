#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>

#include <status.h>
#include <comms.h>
#include <db.h>
#include <pool.h>
#include <queue.h>

#define NUM_OF_THREAD_POOL_WORKERS (5)

bool stop_server = false;

static
void sigint_handler(int signum) {
    (void)signum;

    stop_server = true;
    signal(SIGINT, SIG_DFL);
}

int
main(int argc, char *argv[]) {
    status_t retval = UNINITIALIZED;
    db__db_t assitants_db = {0};
    db__db_t students_db = {0};
    int server_sock = -1;
    uint16_t port = 0;
    pool__pool_t *pool = NULL;
    int incoming_client_sock = -1;
    queue__queue_t *pending_clients = NULL;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s PORT\n", argv[0]);
        retval = INVALID_PARAMETERS;
        goto cleanup;
    }

    port = (uint16_t) strtol(argv[1], (char **) NULL, 10);

    retval = db__read_db("./assitants.txt", &assitants_db);
    if(SUCCESS != retval) {
        goto cleanup;
    }

    retval = db__read_db("./students.txt", &students_db);
    if(SUCCESS != retval) {
        goto cleanup;
    }

    if (SIG_ERR == signal(SIGINT, sigint_handler)) {
        retval = GRADE_SERVER__FAILED_SETTING_SIGINT_HANDLER;
        goto cleanup;
    }

    if(SUCCESS != queue__initialize(&pending_clients)) {
        goto cleanup;
    }

    retval = comms__create_server_sock(&server_sock, port);
    if(SUCCESS != retval) {
        goto cleanup;
    }

    retval = pool__initialize(&pool, NUM_OF_THREAD_POOL_WORKERS, pending_clients);
    if(SUCCESS != retval) {
        goto cleanup;
    }
    while(!stop_server) {
        retval = comms__listen_for_client(server_sock, &incoming_client_sock);
        if(SUCCESS != retval) {
            goto cleanup;
        }
        queue__push(pending_clients, incoming_client_sock);
    }

    retval = SUCCESS;

cleanup:
    pool__destroy(pool);
    db__free_db(&assitants_db);
    db__free_db(&students_db);

    queue__queue_destory(pending_clients);

    return retval;
}
