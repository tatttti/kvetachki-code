/**
 * author: Team "Kvetachki"
 * @file price_policy.c
 * @brief Implementation of price policy operations
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "price_policy.h"
#include "db.h"

extern sqlite3 *g_db;

int price_policy_create(PricePolicy *pp) {
    if (!pp || !pp->urgency_type[0]) return -1;
    
    const char *sql = "INSERT INTO PRICE_POLICY (urgency_type, surcharge_percent, description, updated_at) "
                      "VALUES (?, ?, ?, datetime('now'));";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;
    
    sqlite3_bind_text(stmt, 1, pp->urgency_type, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 2, pp->surcharge_percent);
    sqlite3_bind_text(stmt, 3, pp->description, -1, SQLITE_STATIC);
    
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE) {
        pp->id = sqlite3_last_insert_rowid(g_db);
    }
    
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? 0 : -1;
}

int price_policy_update(PricePolicy *pp) {
    if (!pp || pp->id <= 0) return -1;
    
    const char *sql = "UPDATE PRICE_POLICY SET urgency_type = ?, surcharge_percent = ?, "
                      "description = ?, updated_at = datetime('now') WHERE id = ?;";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;
    
    sqlite3_bind_text(stmt, 1, pp->urgency_type, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 2, pp->surcharge_percent);
    sqlite3_bind_text(stmt, 3, pp->description, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, pp->id);
    
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? 0 : -1;
}

int price_policy_delete(int id) {
    if (id <= 0) return -1;
    
    const char *sql = "DELETE FROM PRICE_POLICY WHERE id = ?;";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;
    
    sqlite3_bind_int(stmt, 1, id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? 0 : -1;
}

PricePolicy* price_policy_find_by_id(int id) {
    if (id <= 0) return NULL;
    
    const char *sql = "SELECT id, urgency_type, surcharge_percent, description, updated_at "
                      "FROM PRICE_POLICY WHERE id = ?;";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return NULL;
    
    sqlite3_bind_int(stmt, 1, id);
    
    PricePolicy *pp = NULL;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        pp = (PricePolicy*)malloc(sizeof(PricePolicy));
        pp->id = sqlite3_column_int(stmt, 0);
        strcpy(pp->urgency_type, (const char*)sqlite3_column_text(stmt, 1));
        pp->surcharge_percent = sqlite3_column_double(stmt, 2);
        strcpy(pp->description, (const char*)sqlite3_column_text(stmt, 3));
        strcpy(pp->updated_at, (const char*)sqlite3_column_text(stmt, 4));
    }
    
    sqlite3_finalize(stmt);
    return pp;
}

PricePolicy* price_policy_find_by_urgency(const char *urgency_type) {
    if (!urgency_type) return NULL;
    
    const char *sql = "SELECT id, urgency_type, surcharge_percent, description, updated_at "
                      "FROM PRICE_POLICY WHERE urgency_type = ?;";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return NULL;
    
    sqlite3_bind_text(stmt, 1, urgency_type, -1, SQLITE_STATIC);
    
    PricePolicy *pp = NULL;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        pp = (PricePolicy*)malloc(sizeof(PricePolicy));
        pp->id = sqlite3_column_int(stmt, 0);
        strcpy(pp->urgency_type, (const char*)sqlite3_column_text(stmt, 1));
        pp->surcharge_percent = sqlite3_column_double(stmt, 2);
        strcpy(pp->description, (const char*)sqlite3_column_text(stmt, 3));
        strcpy(pp->updated_at, (const char*)sqlite3_column_text(stmt, 4));
    }
    
    sqlite3_finalize(stmt);
    return pp;
}

PricePolicy** price_policy_find_all(int *count) {
    *count = 0;
    const char *sql = "SELECT id, urgency_type, surcharge_percent, description, updated_at FROM PRICE_POLICY;";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return NULL;
    
    int rows = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) rows++;
    sqlite3_reset(stmt);
    
    if (rows == 0) {
        sqlite3_finalize(stmt);
        return NULL;
    }
    
    PricePolicy **arr = (PricePolicy**)malloc(sizeof(PricePolicy*) * rows);
    int idx = 0;
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        PricePolicy *pp = (PricePolicy*)malloc(sizeof(PricePolicy));
        pp->id = sqlite3_column_int(stmt, 0);
        strcpy(pp->urgency_type, (const char*)sqlite3_column_text(stmt, 1));
        pp->surcharge_percent = sqlite3_column_double(stmt, 2);
        strcpy(pp->description, (const char*)sqlite3_column_text(stmt, 3));
        strcpy(pp->updated_at, (const char*)sqlite3_column_text(stmt, 4));
        arr[idx++] = pp;
    }
    
    *count = idx;
    sqlite3_finalize(stmt);
    return arr;
}

void price_policy_free(PricePolicy *pp) {
    if (pp) free(pp);
}

void price_policy_print(const PricePolicy *pp) {
    if (!pp) return;
    printf("ID: %d | Urgency: %s | Surcharge: %.2f%% | Description: %s | Updated: %s\n",
           pp->id, pp->urgency_type, pp->surcharge_percent, pp->description, pp->updated_at);
}

double get_surcharge_by_urgency(const char *urgency_type) {
    PricePolicy *pp = price_policy_find_by_urgency(urgency_type);
    if (!pp) return 0.0;
    
    double surcharge = pp->surcharge_percent;
    price_policy_free(pp);
    return surcharge;
}
