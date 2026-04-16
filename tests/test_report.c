/**
 * @file test_report.c
 * @brief Unit tests for report module
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "../include/report.h"
#include "../include/order.h"
#include "../include/flower.h"
#include "../include/composition.h"
#include "../include/price_policy.h"
#include "../include/db.h"

sqlite3 *g_db = NULL;

static void setup(void) {
    sqlite3_open(":memory:", &g_db);
    char *err = NULL;
    
    sqlite3_exec(g_db, "CREATE TABLE FLOWERS (id INTEGER PRIMARY KEY, name TEXT, variety TEXT, unit_price REAL);", NULL, NULL, &err);
    sqlite3_exec(g_db, "CREATE TABLE COMPOSITIONS (id INTEGER PRIMARY KEY, name TEXT, description TEXT);", NULL, NULL, &err);
    sqlite3_exec(g_db, "CREATE TABLE COMPOSITION_ITEMS (id INTEGER PRIMARY KEY, composition_id INTEGER, flower_id INTEGER, quantity INTEGER);", NULL, NULL, &err);
    sqlite3_exec(g_db, "CREATE TABLE CUSTOMERS (id INTEGER PRIMARY KEY, name TEXT);", NULL, NULL, &err);
    sqlite3_exec(g_db, "CREATE TABLE ORDERS (id INTEGER PRIMARY KEY, customer_id INTEGER, composition_id INTEGER, quantity INTEGER, urgency TEXT, order_date TEXT, delivery_date TEXT, total_price REAL);", NULL, NULL, &err);
    sqlite3_exec(g_db, "CREATE TABLE PRICE_POLICY (id INTEGER PRIMARY KEY, urgency_type TEXT, surcharge_percent REAL);", NULL, NULL, &err);
    
    /* Insert test data */
    sqlite3_exec(g_db, "INSERT INTO FLOWERS VALUES (1, 'Rose', 'Red', 5.00);", NULL, NULL, &err);
    sqlite3_exec(g_db, "INSERT INTO FLOWERS VALUES (2, 'Tulip', 'Yellow', 3.00);", NULL, NULL, &err);
    sqlite3_exec(g_db, "INSERT INTO COMPOSITIONS VALUES (1, 'Romantic', 'Red roses');", NULL, NULL, &err);
    sqlite3_exec(g_db, "INSERT INTO COMPOSITIONS VALUES (2, 'Spring', 'Tulips');", NULL, NULL, &err);
    sqlite3_exec(g_db, "INSERT INTO COMPOSITION_ITEMS VALUES (1, 1, 1, 5);", NULL, NULL, &err);
    sqlite3_exec(g_db, "INSERT INTO COMPOSITION_ITEMS VALUES (2, 2, 2, 10);", NULL, NULL, &err);
    sqlite3_exec(g_db, "INSERT INTO CUSTOMERS VALUES (1, 'Customer1');", NULL, NULL, &err);
    sqlite3_exec(g_db, "INSERT INTO ORDERS VALUES (1, 1, 1, 2, 'normal', '2025-01-15', '2025-01-20', 50.00);", NULL, NULL, &err);
    sqlite3_exec(g_db, "INSERT INTO ORDERS VALUES (2, 1, 2, 1, '1day', '2025-02-10', '2025-02-11', 37.50);", NULL, NULL, &err);
    sqlite3_exec(g_db, "INSERT INTO PRICE_POLICY VALUES (1, 'normal', 0), (2, '1day', 25), (3, '2days', 15);", NULL, NULL, &err);
}

static void teardown(void) {
    sqlite3_close(g_db);
    g_db = NULL;
}

static void test_revenue_for_period(void) {
    double revenue = select_revenue_for_period("2025-01-01", "2025-01-31");
    if (revenue == 50.00) {
        printf("PASSED: test_revenue_for_period (January)\n");
    } else {
        printf("FAILED: test_revenue_for_period (got %.2f, expected 50.00)\n", revenue);
    }
    
    revenue = select_revenue_for_period("2025-02-01", "2025-02-28");
    if (revenue == 37.50) {
        printf("PASSED: test_revenue_for_period (February)\n");
    } else {
        printf("FAILED: test_revenue_for_period (got %.2f, expected 37.50)\n", revenue);
    }
}

static void test_orders_by_urgency(void) {
    /* Just check that function doesn't crash */
    printf("Testing select_orders_by_urgency (output check manually)...\n");
    select_orders_by_urgency();
    printf("PASSED: test_orders_by_urgency (no crash)\n");
}

int main(void) {
    setup();
    printf("\n=== Report Module Tests ===\n");
    test_revenue_for_period();
    test_orders_by_urgency();
    teardown();
    return 0;
}
