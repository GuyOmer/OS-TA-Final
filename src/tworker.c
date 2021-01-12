#include <tworker.h>
#include <backend.h>
#include <queue.h>
#include <stdio.h>

void *
tworker__worker_routine(void *queue) {
    status_t retval = UNINITIALIZED;
    int client_socket = -1;

    // workers never stop executing
    while(1) {
        retval = queue__pop((queue__queue_t *)queue, &client_socket);
        backend__interact_with_client(client_socket);
        fprintf(stderr, "%d\n", retval);
    }
}
