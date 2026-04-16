/**
 * @file test_flower.c
 * @brief Unit tests for flower module
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "../include/flower.h"
#include "../include/db.h"

sqlite3 *g_db = NULL;

extern sqlite3 *g_db;

static void setup(void) {
    sqlite3_open(":memory:", &g_db);
    const char *sql = "CREATE TABLE FLOWERS ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "name VARCHAR(100) NOT NULL,"
                      "variety VARCHAR(100),"
                      "unit_price DECIMAL(10,2) NOT NULL,"
                      "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP);";
    char *err = NULL;
    sqlite3_exec(g_db, sql, NULL, NULL, &err);
}

static void teardown(void) {
    sqlite3_close(g_db);
    g_db = NULL;
}

static void test_flower_create(void) {
    Flower f;
    memset(&f, 0, sizeof(Flower));
    strcpy(f.name, "Rose");
    strcpy(f.variety, "Red");
    f.unit_price = 5.00;
    
    int rc = flower_create(&f);
    if (rc == 0 && f.id > 0) {
        printf("PASSED: test_flower_create (id=%d)\n", f.id);
    } else {
        printf("FAILED: test_flower_create\n");
    }
}

static void test_flower_find_by_id(void) {
    Flower f;
    memset(&f, 0, sizeof(Flower));
    strcpy(f.name, "Tulip");
    strcpy(f.variety, "Yellow");
    f.unit_price = 3.00;
    flower_create(&f);
    int expected_id = f.id;
    
    Flower *found = flower_find_by_id(expected_id);
    if (found && found->id == expected_id && strcmp(found->name, "Tulip") == 0) {
        printf("PASSED: test_flower_find_by_id\n");
        flower_free(found);
    } else {
        printf("FAILED: test_flower_find_by_id\n");
    }
}

static void test_flower_update(void) {
    Flower f;
    memset(&f, 0, sizeof(Flower));
    strcpy(f.name, "Lily");
    strcpy(f.variety, "White");
    f.unit_price = 4.00;
    flower_create(&f);
    
    f.unit_price = 4.50;
    int rc = flower_update(&f);
    
    Flower *updated = flower_find_by_id(f.id);
    if (rc == 0 && updated && updated->unit_price == 4.50) {
        printf("PASSED: test_flower_update\n");
        flower_free(updated);
    } else {
        printf("FAILED: test_flower_update\n");
    }
}

static void test_flower_delete(void) {
    Flower f;
    memset(&f, 0, sizeof(Flower));
    strcpy(f.name, "Orchid");
    strcpy(f.variety, "Purple");
    f.unit_price = 10.00;
    flower_create(&f);
    int id = f.id;
    
    int rc = flower_delete(id);
    Flower *deleted = flower_find_by_id(id);
    
    if (rc == 0 && deleted == NULL) {
        printf("PASSED: test_flower_delete\n");
    } else {
        printf("FAILED: test_flower_delete\n");
    }
    if (deleted) flower_free(deleted);
}

int main(void) {
    setup();
    printf("\n=== Flower Module Tests ===\n");
    test_flower_create();
    test_flower_find_by_id();
    test_flower_update();
    test_flower_delete();
    teardown();
    return 0;
}
