/**
 * @file test_user.c
 * @brief Unit tests for user module
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "../include/user.h"
#include "../include/db.h"

extern sqlite3 *g_db;

static void setup(void) {
    sqlite3_open(":memory:", &g_db);
    const char *sql = "CREATE TABLE USERS ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "login VARCHAR(50) NOT NULL UNIQUE,"
                      "password_hash VARCHAR(128) NOT NULL,"
                      "role VARCHAR(20) NOT NULL,"
                      "customer_id INTEGER,"
                      "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP);";
    char *err = NULL;
    sqlite3_exec(g_db, sql, NULL, NULL, &err);
}

static void teardown(void) {
    sqlite3_close(g_db);
    g_db = NULL;
}

static void test_user_create(void) {
    User u;
    memset(&u, 0, sizeof(User));
    strcpy(u.login, "testuser");
    strcpy(u.password_hash, "hash123");
    strcpy(u.role, "CUSTOMER");
    u.customer_id = 0;
    
    int rc = user_create(&u);
    if (rc == 0 && u.id > 0) {
        printf("PASSED: test_user_create (id=%d)\n", u.id);
    } else {
        printf("FAILED: test_user_create\n");
    }
}

static void test_user_find_by_login(void) {
    User u;
    memset(&u, 0, sizeof(User));
    strcpy(u.login, "findme");
    strcpy(u.password_hash, "hash456");
    strcpy(u.role, "ADMIN");
    user_create(&u);
    
    User *found = user_find_by_login("findme");
    if (found && strcmp(found->login, "findme") == 0) {
        printf("PASSED: test_user_find_by_login\n");
        user_free(found);
    } else {
        printf("FAILED: test_user_find_by_login\n");
    }
}

static void test_user_authenticate(void) {
    User u;
    memset(&u, 0, sizeof(User));
    strcpy(u.login, "authuser");
    strcpy(u.password_hash, "7572152304808164");
    strcpy(u.role, "CUSTOMER");
    user_create(&u);
    
    int user_id = user_authenticate("authuser", "admin123");
    if (user_id > 0) {
        printf("PASSED: test_user_authenticate (id=%d)\n", user_id);
    } else {
        printf("FAILED: test_user_authenticate\n");
    }
}

int main(void) {
    setup();
    printf("\n=== User Module Tests ===\n");
    test_user_create();
    test_user_find_by_login();
    test_user_authenticate();
    teardown();
    return 0;
}
