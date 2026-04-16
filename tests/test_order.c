/**
 * @file test_order.c
 * @brief Unit tests for order module
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "../include/order.h"
#include "../include/composition.h"
#include "../include/flower.h"
#include "../include/db.h"

extern sqlite3 *g_db;

static void setup(void) {
    sqlite3_open(":memory:", &g_db);
    char *err = NULL;
    sqlite3_exec(g_db, "CREATE TABLE FLOWERS (id INTEGER PRIMARY KEY AUTOINCREMENT, name VARCHAR(100), variety VARCHAR(100), unit_price DECIMAL(10,2), created_at TIMESTAMP);", NULL, NULL, &err);
    sqlite3_exec(g_db, "CREATE TABLE COMPOSITIONS (id INTEGER PRIMARY KEY AUTOINCREMENT, name VARCHAR(100), description TEXT, created_at TIMESTAMP);", NULL, NULL, &err);
    sqlite3_exec(g_db, "CREATE TABLE COMPOSITION_ITEMS (id INTEGER PRIMARY KEY AUTOINCREMENT, composition_id INTEGER, flower_id INTEGER, quantity INTEGER);", NULL, NULL, &err);
    sqlite3_exec(g_db, "CREATE TABLE CUSTOMERS (id INTEGER PRIMARY KEY AUTOINCREMENT, name VARCHAR(100), phone VARCHAR(20), email VARCHAR(100), created_at TIMESTAMP);", NULL, NULL, &err);
    sqlite3_exec(g_db, "CREATE TABLE ORDERS (id INTEGER PRIMARY KEY AUTOINCREMENT, customer_id INTEGER, composition_id INTEGER, quantity INTEGER, urgency VARCHAR(10), order_date DATE, delivery_date DATE, total_price DECIMAL(10,2), created_at TIMESTAMP);", NULL, NULL, &err);
    
    /* Insert test customer */
    sqlite3_exec(g_db, "INSERT INTO CUSTOMERS (id, name) VALUES (1, 'Test Customer');", NULL, NULL, &err);
    /* Insert test flower and composition */
    sqlite3_exec(g_db, "INSERT INTO FLOWERS (id, name, unit_price) VALUES (1, 'Rose', 5.00);", NULL, NULL, &err);
    sqlite3_exec(g_db, "INSERT INTO COMPOSITIONS (id, name) VALUES (1, 'Test Bouquet');", NULL, NULL, &err);
    sqlite3_exec(g_db, "INSERT INTO COMPOSITION_ITEMS (composition_id, flower_id, quantity) VALUES (1, 1, 3);", NULL, NULL, &err);
}

static void teardown(void) {
    sqlite3_close(g_db);
    g_db = NULL;
}

static void test_order_create(void) {
    Order o;
    memset(&o, 0, sizeof(Order));
    o.customer_id = 1;
    o.composition_id = 1;
    o.quantity = 2;
    strcpy(o.urgency, "normal");
    strcpy(o.order_date, "2025-01-01");
    strcpy(o.delivery_date, "2025-01-05");
    o.total_price = 30.00;
    
    int rc = order_create(&o);
    if (rc == 0 && o.id > 0) {
        printf("PASSED: test_order_create (id=%d)\n", o.id);
    } else {
        printf("FAILED: test_order_create\n");
    }
}

static void test_order_find_by_id(void) {
    Order o;
    memset(&o, 0, sizeof(Order));
    o.customer_id = 1;
    o.composition_id = 1;
    o.quantity = 1;
    strcpy(o.urgency, "1day");
    strcpy(o.order_date, "2025-01-10");
    strcpy(o.delivery_date, "2025-01-11");
    o.total_price = 18.75;
    order_create(&o);
    int expected_id = o.id;
    
    Order *found = order_find_by_id(expected_id);
    if (found && found->id == expected_id) {
        printf("PASSED: test_order_find_by_id\n");
        order_free(found);
    } else {
        printf("FAILED: test_order_find_by_id\n");
    }
}

static void test_order_delete(void) {
    Order o;
    memset(&o, 0, sizeof(Order));
    o.customer_id = 1;
    o.composition_id = 1;
    o.quantity = 1;
    strcpy(o.urgency, "2days");
    strcpy(o.order_date, "2025-02-01");
    strcpy(o.delivery_date, "2025-02-03");
    o.total_price = 17.25;
    order_create(&o);
    int id = o.id;
    
    int rc = order_delete(id);
    Order *deleted = order_find_by_id(id);
    
    if (rc == 0 && deleted == NULL) {
        printf("PASSED: test_order_delete\n");
    } else {
        printf("FAILED: test_order_delete\n");
    }
    if (deleted) order_free(deleted);
}

int main(void) {
    setup();
    printf("\n=== Order Module Tests ===\n");
    test_order_create();
    test_order_find_by_id();
    test_order_delete();
    teardown();
    return 0;
}
