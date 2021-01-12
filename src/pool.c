#include <stddef.h>
#include <pthread.h>

#include <status.h>
#include <tworker.h>
#include <pool.h>
#include <stdlib.h>
#include <memory.h>
#include <common.h>

status_t
pool__initialize(pool__pool_t **pool, size_t pool_size, void *thread_args) {
    status_t retval = UNINITIALIZED;
    pool__pool_t *inner_pool = NULL;
    size_t pool_workers_size = 0;
    size_t i = 0;

    inner_pool = (pool__pool_t *)malloc(sizeof(*inner_pool));
    if(inner_pool == NULL) {
        retval = POOL__FAILED_INITIALIZING_POOL;
        goto cleanup;
    }

    pool_workers_size = sizeof(*inner_pool->workers) * pool_size;
    inner_pool->workers = malloc(pool_workers_size);
    if(inner_pool->workers == NULL) {
        retval = POOL__FAILED_ALLOCATING_WORKERS;
        goto cleanup;
    }

    inner_pool->size = pool_size;

    for(i = 0; i < inner_pool->size; ++i) {
        // threads are cancelable by default, so no need to modify attrs
        if(0 != pthread_create(&inner_pool->workers[i], NULL, tworker__worker_routine, thread_args)) {
            retval = POOL__FAILED_CREATING_WORKER;
            goto cleanup;
        }
    }

    *pool = inner_pool;
    inner_pool = NULL;
    retval = SUCCESS;

cleanup:
    pool__destroy(inner_pool);
    return retval;

}

status_t
pool__destroy(pool__pool_t *pool) {
    // start with success and overwrite with status if needed
    status_t retval = SUCCESS;
    size_t i = 0;

    if(pool == NULL) {
        retval = INVALID_PARAMETERS;
        return retval;
    }

    if(pool->workers != NULL) {
        for (i = 0; i < pool->size; ++i) {
            if (0 != pthread_cancel(pool->workers[i])) {
                // alltough we failed, continue canceling threads to free as much resources as possible
                retval = POOL__FAILED_DESTROYING_POOL;
            }
        }
    }

    SAFE_FREE(pool->workers);
    SAFE_FREE(pool);

    return retval;
}
