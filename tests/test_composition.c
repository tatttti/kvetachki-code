/**
 * @file test_composition.c
 * @brief Unit tests for composition module
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "../include/composition.h"
#include "../include/flower.h"
#include "../include/db.h"

extern sqlite3 *g_db;

static void setup(void) {
    sqlite3_open(":memory:", &g_db);
    char *err = NULL;
    sqlite3_exec(g_db, "CREATE TABLE FLOWERS ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "name VARCHAR(100) NOT NULL,"
                      "variety VARCHAR(100),"
                      "unit_price DECIMAL(10,2) NOT NULL,"
                      "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP);", NULL, NULL, &err);
    sqlite3_exec(g_db, "CREATE TABLE COMPOSITIONS ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "name VARCHAR(100) NOT NULL,"
                      "description TEXT,"
                      "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP);", NULL, NULL, &err);
    sqlite3_exec(g_db, "CREATE TABLE COMPOSITION_ITEMS ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "composition_id INTEGER NOT NULL,"
                      "flower_id INTEGER NOT NULL,"
                      "quantity INTEGER NOT NULL,"
                      "FOREIGN KEY (composition_id) REFERENCES COMPOSITIONS(id),"
                      "FOREIGN KEY (flower_id) REFERENCES FLOWERS(id));", NULL, NULL, &err);
}

static void teardown(void) {
    sqlite3_close(g_db);
    g_db = NULL;
}

static void test_composition_create(void) {
    Composition c;
    memset(&c, 0, sizeof(Composition));
    strcpy(c.name, "Romantic Bouquet");
    strcpy(c.description, "Red roses bouquet");
    
    int rc = composition_create(&c);
    if (rc == 0 && c.id > 0) {
        printf("PASSED: test_composition_create (id=%d)\n", c.id);
    } else {
        printf("FAILED: test_composition_create\n");
    }
}

static void test_composition_find_by_id(void) {
    Composition c;
    memset(&c, 0, sizeof(Composition));
    strcpy(c.name, "Spring Bouquet");
    strcpy(c.description, "Tulips and lilies");
    composition_create(&c);
    int expected_id = c.id;
    
    Composition *found = composition_find_by_id(expected_id);
    if (found && found->id == expected_id && strcmp(found->name, "Spring Bouquet") == 0) {
        printf("PASSED: test_composition_find_by_id\n");
        composition_free(found);
    } else {
        printf("FAILED: test_composition_find_by_id\n");
    }
}

static void test_composition_update(void) {
    Composition c;
    memset(&c, 0, sizeof(Composition));
    strcpy(c.name, "Old Name");
    strcpy(c.description, "Old description");
    composition_create(&c);
    
    strcpy(c.name, "New Name");
    strcpy(c.description, "New description");
    int rc = composition_update(&c);
    
    Composition *updated = composition_find_by_id(c.id);
    if (rc == 0 && updated && strcmp(updated->name, "New Name") == 0) {
        printf("PASSED: test_composition_update\n");
        composition_free(updated);
    } else {
        printf("FAILED: test_composition_update\n");
    }
}

int main(void) {
    setup();
    printf("\n=== Composition Module Tests ===\n");
    test_composition_create();
    test_composition_find_by_id();
    test_composition_update();
    teardown();
    return 0;
}
