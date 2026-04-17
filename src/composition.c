/**
 * author: Team "Kvetachki"
 * @file composition.c
 * @brief Implementation of composition CRUD operations
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "composition.h"
#include "db.h"

extern sqlite3 *g_db;

int composition_create(Composition *c) {
    if (!c || !c->name[0]) return -1;
    
    const char *sql = "INSERT INTO COMPOSITIONS (name, description, created_at) "
                      "VALUES (?, ?, datetime('now'));";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;
    
    sqlite3_bind_text(stmt, 1, c->name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, c->description, -1, SQLITE_STATIC);
    
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE) {
        c->id = sqlite3_last_insert_rowid(g_db);
    }
    
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? 0 : -1;
}

int composition_update(Composition *c) {
    if (!c || c->id <= 0) return -1;
    
    const char *sql = "UPDATE COMPOSITIONS SET name = ?, description = ? WHERE id = ?;";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;
    
    sqlite3_bind_text(stmt, 1, c->name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, c->description, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, c->id);
    
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? 0 : -1;
}

int composition_delete(int id) {
    if (id <= 0) return -1;
    
    /* First delete all composition items */
    const char *del_items = "DELETE FROM COMPOSITION_ITEMS WHERE composition_id = ?;";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(g_db, del_items, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
    
    /* Then delete the composition itself */
    const char *sql = "DELETE FROM COMPOSITIONS WHERE id = ?;";
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;
    
    sqlite3_bind_int(stmt, 1, id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? 0 : -1;
}

Composition* composition_find_by_id(int id) {
    if (id <= 0) return NULL;
    
    const char *sql = "SELECT id, name, description, created_at FROM COMPOSITIONS WHERE id = ?;";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return NULL;
    
    sqlite3_bind_int(stmt, 1, id);
    
    Composition *c = NULL;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        c = (Composition*)malloc(sizeof(Composition));
        c->id = sqlite3_column_int(stmt, 0);
        strcpy(c->name, (const char*)sqlite3_column_text(stmt, 1));
        strcpy(c->description, (const char*)sqlite3_column_text(stmt, 2));
        strcpy(c->created_at, (const char*)sqlite3_column_text(stmt, 3));
    }
    
    sqlite3_finalize(stmt);
    return c;
}

Composition** composition_find_all(int *count) {
    *count = 0;
    const char *sql = "SELECT id, name, description, created_at FROM COMPOSITIONS;";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return NULL;
    
    /* First count rows */
    int rows = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) rows++;
    sqlite3_reset(stmt);
    
    if (rows == 0) {
        sqlite3_finalize(stmt);
        return NULL;
    }
    
    Composition **arr = (Composition**)malloc(sizeof(Composition*) * rows);
    int idx = 0;
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Composition *c = (Composition*)malloc(sizeof(Composition));
        c->id = sqlite3_column_int(stmt, 0);
        strcpy(c->name, (const char*)sqlite3_column_text(stmt, 1));
        strcpy(c->description, (const char*)sqlite3_column_text(stmt, 2));
        strcpy(c->created_at, (const char*)sqlite3_column_text(stmt, 3));
        arr[idx++] = c;
    }
    
    *count = idx;
    sqlite3_finalize(stmt);
    return arr;
}

void composition_free(Composition *c) {
    if (c) free(c);
}

void composition_print(const Composition *c) {
    if (!c) return;
    printf("ID: %d | Name: %s | Description: %s | Created: %s\n",
           c->id, c->name, c->description, c->created_at);
}

int composition_add_item(int composition_id, int flower_id, int quantity) {
    if (composition_id <= 0 || flower_id <= 0 || quantity <= 0) return -1;
    
    const char *sql = "INSERT INTO COMPOSITION_ITEMS (composition_id, flower_id, quantity) "
                      "VALUES (?, ?, ?);";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;
    
    sqlite3_bind_int(stmt, 1, composition_id);
    sqlite3_bind_int(stmt, 2, flower_id);
    sqlite3_bind_int(stmt, 3, quantity);
    
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? 0 : -1;
}

int composition_remove_item(int composition_id, int flower_id) {
    const char *sql = "DELETE FROM COMPOSITION_ITEMS WHERE composition_id = ? AND flower_id = ?;";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;
    
    sqlite3_bind_int(stmt, 1, composition_id);
    sqlite3_bind_int(stmt, 2, flower_id);
    
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? 0 : -1;
}

CompositionItem** composition_get_items(int composition_id, int *count) {
    *count = 0;
    const char *sql = "SELECT flower_id, quantity FROM COMPOSITION_ITEMS WHERE composition_id = ?;";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return NULL;
    
    sqlite3_bind_int(stmt, 1, composition_id);
    
    /* Count rows */
    int rows = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) rows++;
    sqlite3_reset(stmt);
    
    if (rows == 0) {
        sqlite3_finalize(stmt);
        return NULL;
    }
    
    CompositionItem **items = (CompositionItem**)malloc(sizeof(CompositionItem*) * rows);
    int idx = 0;
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        CompositionItem *item = (CompositionItem*)malloc(sizeof(CompositionItem));
        item->flower_id = sqlite3_column_int(stmt, 0);
        item->quantity = sqlite3_column_int(stmt, 1);
        items[idx++] = item;
    }
    
    *count = idx;
    sqlite3_finalize(stmt);
    return items;
}

void composition_items_free(CompositionItem **items, int count) {
    if (!items) return;
    for (int i = 0; i < count; i++) {
        if (items[i]) free(items[i]);
    }
    free(items);
}
