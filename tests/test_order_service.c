/**
 * @file test_order_service.c
 * @brief Unit tests for order service module
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "../include/order_service.h"
#include "../include/order.h"
#include "../include/composition.h"
#include "../include/flower.h"
#include "../include/price_policy.h"
#include "../include/db.h"

sqlite3 *g_db = NULL;

static void setup(void) {
    sqlite3_open(":memory:", &g_db);
    char *err = NULL;

    /* Create tables */
    sqlite3_exec(g_db, "CREATE TABLE FLOWERS (id INTEGER PRIMARY KEY, name TEXT, variety TEXT, unit_price REAL, created_at TEXT);", NULL, NULL, &err);
    sqlite3_exec(g_db, "CREATE TABLE COMPOSITIONS (id INTEGER PRIMARY KEY, name TEXT, description TEXT, created_at TEXT);", NULL, NULL, &err);
    sqlite3_exec(g_db, "CREATE TABLE COMPOSITION_ITEMS (id INTEGER PRIMARY KEY, composition_id INTEGER, flower_id INTEGER, quantity INTEGER);", NULL, NULL, &err);
    sqlite3_exec(g_db, "CREATE TABLE CUSTOMERS (id INTEGER PRIMARY KEY, name TEXT, phone TEXT, email TEXT, created_at TEXT);", NULL, NULL, &err);
    sqlite3_exec(g_db, "CREATE TABLE ORDERS (id INTEGER PRIMARY KEY, customer_id INTEGER, composition_id INTEGER, quantity INTEGER, urgency TEXT, order_date TEXT, delivery_date TEXT, total_price REAL, created_at TEXT);", NULL, NULL, &err);
    sqlite3_exec(g_db, "CREATE TABLE ORDER_SUMMARIES (id INTEGER PRIMARY KEY, order_id INTEGER, final_price REAL, calculated_at TEXT);", NULL, NULL, &err);
    sqlite3_exec(g_db, "CREATE TABLE PRICE_POLICY (id INTEGER PRIMARY KEY, urgency_type TEXT, surcharge_percent REAL, description TEXT, updated_at TEXT);", NULL, NULL, &err);

    /* Insert test data */
    sqlite3_exec(g_db, "INSERT INTO FLOWERS (id, name, unit_price) VALUES (1, 'Rose', 5.00);", NULL, NULL, &err);
    sqlite3_exec(g_db, "INSERT INTO FLOWERS (id, name, unit_price) VALUES (2, 'Tulip', 3.00);", NULL, NULL, &err);
    sqlite3_exec(g_db, "INSERT INTO COMPOSITIONS (id, name) VALUES (1, 'Test Bouquet');", NULL, NULL, &err);
    sqlite3_exec(g_db, "INSERT INTO COMPOSITION_ITEMS (composition_id, flower_id, quantity) VALUES (1, 1, 3);", NULL, NULL, &err);
    sqlite3_exec(g_db, "INSERT INTO COMPOSITION_ITEMS (composition_id, flower_id, quantity) VALUES (1, 2, 2);", NULL, NULL, &err);
    sqlite3_exec(g_db, "INSERT INTO CUSTOMERS (id, name) VALUES (1, 'Test Customer');", NULL, NULL, &err);
    sqlite3_exec(g_db, "INSERT INTO PRICE_POLICY (urgency_type, surcharge_percent) VALUES ('normal', 0), ('1day', 25), ('2days', 15);", NULL, NULL, &err);
}

static void teardown(void) {
    sqlite3_close(g_db);
    g_db = NULL;
}

static void test_validate_order(void) {
    int valid = validate_order(1, 5);
    if (valid == 1) {
        printf("PASSED: test_validate_order (valid composition)\n");
    } else {
        printf("FAILED: test_validate_order (valid composition)\n");
    }

    valid = validate_order(999, 5);
    if (valid == 0) {
        printf("PASSED: test_validate_order (invalid composition)\n");
    } else {
        printf("FAILED: test_validate_order (invalid composition)\n");
    }

    valid = validate_order(1, 0);
    if (valid == 0) {
        printf("PASSED: test_validate_order (zero quantity)\n");
    } else {
        printf("FAILED: test_validate_order (zero quantity)\n");
    }
}

static void test_calculate_order_total(void) {
    double total = calculate_order_total(1, 1, "normal");
    double expected = (5.00 * 3 + 3.00 * 2) * 1;
    if (total == expected) {
        printf("PASSED: test_calculate_order_total (normal, qty=1)\n");
    } else {
        printf("FAILED: test_calculate_order_total (got %.2f, expected %.2f)\n", total, expected);
    }

    total = calculate_order_total(1, 2, "1day");
    expected = ((5.00 * 3 + 3.00 * 2) * 2) * 1.25;
    if (total == expected) {
        printf("PASSED: test_calculate_order_total (1day, qty=2)\n");
    } else {
        printf("FAILED: test_calculate_order_total (got %.2f, expected %.2f)\n", total, expected);
    }
}

static void test_process_order(void) {
    int order_id = process_order(1, 1, 2, "normal", "2025-05-01");
    if (order_id > 0) {
        printf("PASSED: test_process_order (order_id=%d)\n", order_id);
    } else {
        printf("FAILED: test_process_order\n");
    }

    /* Verify order was saved */
    Order *o = order_find_by_id(order_id);
    if (o && o->customer_id == 1 && o->composition_id == 1 && o->quantity == 2) {
        printf("PASSED: test_process_order (order saved correctly)\n");
        order_free(o);
    } else {
        printf("FAILED: test_process_order (order not saved correctly)\n");
    }

    /* Test invalid order */
    int invalid_id = process_order(999, 1, 1, "normal", "2025-05-01");
    if (invalid_id == -1) {
        printf("PASSED: test_process_order (invalid customer)\n");
    } else {
        printf("FAILED: test_process_order (invalid customer)\n");
    }
}

int main(void) {
    setup();
    printf("\n=== Order Service Module Tests ===\n");
    test_validate_order();
    test_calculate_order_total();
    test_process_order();
    teardown();
    return 0;
}
