#include <stdlib.h>

#include <stddef.h>
#include <status.h>
#include <db.h>
#include <common.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define MAX_DB_LINE_LENGTH (256)

status_t
db__read_db(const char *db_path, db__db_t *db) {
    status_t retval = UNINITIALIZED;
    FILE *db_file = NULL;
    size_t db_entries_capacity = 0;
    size_t amount_of_current_entries = 0;
    db__user_entry_t *inner_entries = NULL;
    char line_buffer[MAX_DB_LINE_LENGTH] = {0};
    char *id = NULL;
    char *password = NULL;

    db_file = fopen(db_path, "r");
    if (db_file == NULL) {
        retval = DB__FAILED_OPENING_DB_FILE;
        goto cleanup;
    }

    while(fgets(line_buffer, STRING_LENGTH(line_buffer), db_file)) {
        ++amount_of_current_entries;

        if (amount_of_current_entries > db_entries_capacity) {
            db_entries_capacity = db_entries_capacity == 0 ? 1 : db_entries_capacity * 2;

            inner_entries = realloc(inner_entries, db_entries_capacity * sizeof(*inner_entries));
            if(NULL == inner_entries) {
                retval = DB__FAILED_ALLOCATING_DB;
                goto cleanup;
            }
        }

        id = strtok(line_buffer, DB_ENTRY_DELIM);
        password = strtok(NULL, DB_ENTRY_DELIM);

        inner_entries[amount_of_current_entries - 1].id = (char *)malloc(strlen(id) + 1);
        inner_entries[amount_of_current_entries - 1].password = (char *)malloc(strlen(password) + 1);
        strcpy(inner_entries[amount_of_current_entries - 1].id, id);
        strcpy(inner_entries[amount_of_current_entries - 1].password, password);
    }

    // Resize db to fit
    if(NULL == realloc(inner_entries, db_entries_capacity * sizeof(*inner_entries))) {
        retval = DB__FAILED_FITTING_DB;
        goto cleanup;
    };
    retval = SUCCESS;

cleanup:
    db->entries = inner_entries;
    db->amount_of_entries = amount_of_current_entries;
    if (retval != SUCCESS) {
        db__free_db(db);
    }

    SAFE_FCLOSE(db_file);
    return retval;
}

void
db__free_db(db__db_t *db) {
    size_t i = 0;

    for(i=0; i < db->amount_of_entries; ++i) {
        SAFE_FREE(db->entries[i].id);
        SAFE_FREE(db->entries[i].password);
    }

    SAFE_FREE(db->entries);
}