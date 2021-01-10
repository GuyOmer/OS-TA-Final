#pragma once

#define DB_ENTRY_DELIM ":"

typedef struct db__user_entry_s {
    char *id;
    char *password;
} db__user_entry_t;

typedef struct db__db_s {
    db__user_entry_t *entries;
    size_t amount_of_entries;
} db__db_t;

status_t
db__read_db(const char *db_path, db__db_t *db);

void
db__free_db(db__db_t *db);