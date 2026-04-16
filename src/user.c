/**
 * author: Team "Kvetachki"
 * @file user.c
 * @brief Implementation of user authentication
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "user.h"
#include "db.h"

extern sqlite3 *g_db;

/* Simple hash function (for demo only – use proper hashing in production) */
void hash_password(const char *password, char *output, int size) {
    unsigned long hash = 5381;
    int c;
    while ((c = *password++)) {
        hash = ((hash << 5) + hash) + c;
    }
    snprintf(output, size, "%lu", hash);
}

int user_authenticate(const char *login, const char *password) {
    if (!login || !password) return 0;
    
    char hashed[129];
    hash_password(password, hashed, sizeof(hashed));
    
    const char *sql = "SELECT id FROM USERS WHERE login = ? AND password_hash = ?;";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return 0;
    
    sqlite3_bind_text(stmt, 1, login, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, hashed, -1, SQLITE_STATIC);
    
    int user_id = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        user_id = sqlite3_column_int(stmt, 0);
    }
    
    sqlite3_finalize(stmt);
    return user_id;
}

int user_get_role(const char *login, char *role, int size) {
    if (!login || !role || size <= 0) return -1;
    
    const char *sql = "SELECT role FROM USERS WHERE login = ?;";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;
    
    sqlite3_bind_text(stmt, 1, login, -1, SQLITE_STATIC);
    
    int rc = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *r = (const char*)sqlite3_column_text(stmt, 0);
        strncpy(role, r, size - 1);
        role[size - 1] = '\0';
        rc = 0;
    }
    
    sqlite3_finalize(stmt);
    return rc;
}

int user_get_id_by_login(const char *login) {
    if (!login) return -1;
    
    const char *sql = "SELECT id FROM USERS WHERE login = ?;";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;
    
    sqlite3_bind_text(stmt, 1, login, -1, SQLITE_STATIC);
    
    int user_id = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        user_id = sqlite3_column_int(stmt, 0);
    }
    
    sqlite3_finalize(stmt);
    return user_id;
}

int user_create(User *u) {
    if (!u || !u->login[0] || !u->password_hash[0]) return -1;
    
    const char *sql = "INSERT INTO USERS (login, password_hash, role, created_at) "
                      "VALUES (?, ?, ?, datetime('now'));";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;
    
    sqlite3_bind_text(stmt, 1, u->login, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, u->password_hash, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, u->role, -1, SQLITE_STATIC);
    
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE) {
        u->id = sqlite3_last_insert_rowid(g_db);
    }
    
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? 0 : -1;
}

int user_update(User *u) {
    if (!u || u->id <= 0) return -1;
    
    const char *sql = "UPDATE USERS SET login = ?, password_hash = ?, role = ? WHERE id = ?;";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;
    
    sqlite3_bind_text(stmt, 1, u->login, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, u->password_hash, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, u->role, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, u->id);
    
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? 0 : -1;
}

int user_delete(int id) {
    if (id <= 0) return -1;
    
    const char *sql = "DELETE FROM USERS WHERE id = ?;";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;
    
    sqlite3_bind_int(stmt, 1, id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? 0 : -1;
}

User* user_find_by_id(int id) {
    if (id <= 0) return NULL;
    
    const char *sql = "SELECT id, login, password_hash, role, created_at FROM USERS WHERE id = ?;";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return NULL;
    
    sqlite3_bind_int(stmt, 1, id);
    
    User *u = NULL;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        u = (User*)malloc(sizeof(User));
        u->id = sqlite3_column_int(stmt, 0);
        strcpy(u->login, (const char*)sqlite3_column_text(stmt, 1));
        strcpy(u->password_hash, (const char*)sqlite3_column_text(stmt, 2));
        strcpy(u->role, (const char*)sqlite3_column_text(stmt, 3));
        strcpy(u->created_at, (const char*)sqlite3_column_text(stmt, 4));
    }
    
    sqlite3_finalize(stmt);
    return u;
}

User* user_find_by_login(const char *login) {
    if (!login) return NULL;
    
    const char *sql = "SELECT id, login, password_hash, role, created_at FROM USERS WHERE login = ?;";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return NULL;
    
    sqlite3_bind_text(stmt, 1, login, -1, SQLITE_STATIC);
    
    User *u = NULL;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        u = (User*)malloc(sizeof(User));
        u->id = sqlite3_column_int(stmt, 0);
        strcpy(u->login, (const char*)sqlite3_column_text(stmt, 1));
        strcpy(u->password_hash, (const char*)sqlite3_column_text(stmt, 2));
        strcpy(u->role, (const char*)sqlite3_column_text(stmt, 3));
        strcpy(u->created_at, (const char*)sqlite3_column_text(stmt, 4));
    }
    
    sqlite3_finalize(stmt);
    return u;
}

void user_free(User *u) {
    if (u) free(u);
}

void user_print(const User *u) {
    if (!u) return;
    printf("ID: %d | Login: %s | Role: %s | Created: %s\n",
           u->id, u->login, u->role, u->created_at);
}
