/**
 * author: Team "Kvetachki"
 * @file customer.c
 * @brief Customer CRUD implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "customer.h"
#include "db.h"

extern sqlite3 *g_db;

int customer_create(Customer *c) {
    if (!c || !c->name[0]) return -1;

    const char *sql = "INSERT INTO CUSTOMERS (name, phone, email, created_at) "
    "VALUES (?, ?, ?, datetime('now'));";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;

    sqlite3_bind_text(stmt, 1, c->name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, c->phone, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, c->email, -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE) {
        c->id = sqlite3_last_insert_rowid(g_db);
    }

    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? 0 : -1;
}

int customer_update(Customer *c) {
    if (!c || c->id <= 0) return -1;

    const char *sql = "UPDATE CUSTOMERS SET name = ?, phone = ?, email = ? WHERE id = ?;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;

    sqlite3_bind_text(stmt, 1, c->name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, c->phone, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, c->email, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, c->id);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? 0 : -1;
}

int customer_delete(int id) {
    if (id <= 0) return -1;

    const char *sql = "DELETE FROM CUSTOMERS WHERE id = ?;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;

    sqlite3_bind_int(stmt, 1, id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? 0 : -1;
}

Customer* customer_find_by_id(int id) {
    if (id <= 0) return NULL;

    const char *sql = "SELECT id, name, phone, email, created_at FROM CUSTOMERS WHERE id = ?;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return NULL;

    sqlite3_bind_int(stmt, 1, id);

    Customer *c = NULL;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        c = (Customer*)malloc(sizeof(Customer));
        c->id = sqlite3_column_int(stmt, 0);
        strcpy(c->name, (const char*)sqlite3_column_text(stmt, 1));
        strcpy(c->phone, (const char*)sqlite3_column_text(stmt, 2));
        strcpy(c->email, (const char*)sqlite3_column_text(stmt, 3));
        strcpy(c->created_at, (const char*)sqlite3_column_text(stmt, 4));
    }

    sqlite3_finalize(stmt);
    return c;
}

Customer** customer_find_all(int *count) {
    *count = 0;
    const char *sql = "SELECT id, name, phone, email, created_at FROM CUSTOMERS;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return NULL;

    int rows = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) rows++;
    sqlite3_reset(stmt);

    if (rows == 0) {
        sqlite3_finalize(stmt);
        return NULL;
    }

    Customer **arr = (Customer**)malloc(sizeof(Customer*) * rows);
    int idx = 0;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Customer *c = (Customer*)malloc(sizeof(Customer));
        c->id = sqlite3_column_int(stmt, 0);
        strcpy(c->name, (const char*)sqlite3_column_text(stmt, 1));
        strcpy(c->phone, (const char*)sqlite3_column_text(stmt, 2));
        strcpy(c->email, (const char*)sqlite3_column_text(stmt, 3));
        strcpy(c->created_at, (const char*)sqlite3_column_text(stmt, 4));
        arr[idx++] = c;
    }

    *count = idx;
    sqlite3_finalize(stmt);
    return arr;
}

void customer_free(Customer *c) {
    if (c) free(c);
}

void customer_print(const Customer *c) {
    if (!c) return;
    printf("ID: %d | Name: %s | Phone: %s | Email: %s\n",
           c->id, c->name, c->phone, c->email);
}
