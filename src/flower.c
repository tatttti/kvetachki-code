/**
 * author: Team "Kvetachki"
 * @file flower.c
 * @brief Implementation of flower CRUD operations
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "flower.h"
#include "db.h"

extern sqlite3 *g_db;  /* Global database connection (defined in main.c) */

int flower_create(Flower *f) {
    if (!f || !f->name[0]) return -1;
    
    const char *sql = "INSERT INTO FLOWERS (name, variety, unit_price, created_at) "
                      "VALUES (?, ?, ?, datetime('now'));";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;
    
    sqlite3_bind_text(stmt, 1, f->name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, f->variety, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 3, f->unit_price);
    
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE) {
        f->id = sqlite3_last_insert_rowid(g_db);
    }
    
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? 0 : -1;
}

int flower_update(Flower *f) {
    if (!f || f->id <= 0) return -1;
    
    const char *sql = "UPDATE FLOWERS SET name = ?, variety = ?, unit_price = ? "
                      "WHERE id = ?;";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;
    
    sqlite3_bind_text(stmt, 1, f->name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, f->variety, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 3, f->unit_price);
    sqlite3_bind_int(stmt, 4, f->id);
    
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? 0 : -1;
}

int flower_delete(int id) {
    if (id <= 0) return -1;
    
    const char *sql = "DELETE FROM FLOWERS WHERE id = ?;";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;
    
    sqlite3_bind_int(stmt, 1, id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? 0 : -1;
}

Flower* flower_find_by_id(int id) {
    if (id <= 0) return NULL;
    
    const char *sql = "SELECT id, name, variety, unit_price, created_at FROM FLOWERS WHERE id = ?;";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return NULL;
    
    sqlite3_bind_int(stmt, 1, id);
    
    Flower *f = NULL;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        f = (Flower*)malloc(sizeof(Flower));
        f->id = sqlite3_column_int(stmt, 0);
        strcpy(f->name, (const char*)sqlite3_column_text(stmt, 1));
        strcpy(f->variety, (const char*)sqlite3_column_text(stmt, 2));
        f->unit_price = sqlite3_column_double(stmt, 3);
        strcpy(f->created_at, (const char*)sqlite3_column_text(stmt, 4));
    }
    
    sqlite3_finalize(stmt);
    return f;
}

Flower* flower_find_by_name(const char *name) {
    if (!name) return NULL;
    
    const char *sql = "SELECT id, name, variety, unit_price, created_at FROM FLOWERS WHERE name = ?;";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return NULL;
    
    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    
    Flower *f = NULL;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        f = (Flower*)malloc(sizeof(Flower));
        f->id = sqlite3_column_int(stmt, 0);
        strcpy(f->name, (const char*)sqlite3_column_text(stmt, 1));
        strcpy(f->variety, (const char*)sqlite3_column_text(stmt, 2));
        f->unit_price = sqlite3_column_double(stmt, 3);
        strcpy(f->created_at, (const char*)sqlite3_column_text(stmt, 4));
    }
    
    sqlite3_finalize(stmt);
    return f;
}

void flower_free(Flower *f) {
    if (f) free(f);
}

void flower_print(const Flower *f) {
    if (!f) return;
    printf("ID: %d | Name: %s | Variety: %s | Price: %.2f | Created: %s\n",
           f->id, f->name, f->variety, f->unit_price, f->created_at);
}
