#pragma once

typedef struct pool__pool_s {
    size_t size;
    pthread_t *workers;
} pool__pool_t;


status_t
pool__destroy(pool__pool_t *pool);

status_t
pool__initialize(pool__pool_t **pool, size_t pool_size, void *thread_args);