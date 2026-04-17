/**
 * author: Team "Kvetachki"
 * @file db.c
 * @brief Database connection and utility functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "db.h"

/* External global database connection (defined in main.c) */
extern sqlite3 *g_db;

int db_open(const char *filename, sqlite3 **db) {
    if (sqlite3_open(filename, db) != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(*db));
        return -1;
    }
    return 0;
}

void db_close(sqlite3 *db) {
    if (db) {
        sqlite3_close(db);
    }
}

int db_execute(sqlite3 *db, const char *sql, char **err_msg) {
    if (sqlite3_exec(db, sql, NULL, NULL, err_msg) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg ? *err_msg : "unknown");
        return -1;
    }
    return 0;
}
