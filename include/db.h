/**
 * author: Team "Kvetachki"
 * @file db.h
 * @brief Database connection and utility functions
 */

#ifndef DB_H
#define DB_H

#include <sqlite3.h>

/* Database connection functions */
int db_open(const char *filename, sqlite3 **db);
void db_close(sqlite3 *db);
int db_execute(sqlite3 *db, const char *sql, char **err_msg);

/* Global database pointer (defined in main.c) */
extern sqlite3 *g_db;

#endif /* DB_H */
