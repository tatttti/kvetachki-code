#include <stdio.h>
#include <sqlite3.h>
#include "../include/order_service.h"
#include "../include/db.h"

sqlite3 *g_db = NULL;

static void setup(void) {
    sqlite3_open(":memory:", &g_db);
    char *err = NULL;
    sqlite3_exec(g_db, "CREATE TABLE FLOWERS (id INTEGER PRIMARY KEY, name TEXT, unit_price REAL);", NULL, NULL, &err);
    sqlite3_exec(g_db, "CREATE TABLE COMPOSITIONS (id INTEGER PRIMARY KEY, name TEXT);", NULL, NULL, &err);
    sqlite3_exec(g_db, "CREATE TABLE COMPOSITION_ITEMS (composition_id INTEGER, flower_id INTEGER, quantity INTEGER);", NULL, NULL, &err);
    sqlite3_exec(g_db, "CREATE TABLE CUSTOMERS (id INTEGER PRIMARY KEY);", NULL, NULL, &err);
    sqlite3_exec(g_db, "CREATE TABLE ORDERS (id INTEGER PRIMARY KEY, customer_id INTEGER, composition_id INTEGER, quantity INTEGER, urgency TEXT, order_date TEXT, delivery_date TEXT, total_price REAL);", NULL, NULL, &err);
    sqlite3_exec(g_db, "INSERT INTO FLOWERS VALUES (1, 'Rose', 5.00);", NULL, NULL, &err);
    sqlite3_exec(g_db, "INSERT INTO COMPOSITIONS VALUES (1, 'Bouquet');", NULL, NULL, &err);
    sqlite3_exec(g_db, "INSERT INTO COMPOSITION_ITEMS VALUES (1, 1, 3);", NULL, NULL, &err);
    sqlite3_exec(g_db, "INSERT INTO CUSTOMERS VALUES (1);", NULL, NULL, &err);
}

static void teardown(void) {
    sqlite3_close(g_db);
}

static void test_calculate_order_total(void) {
    double total = calculate_order_total(1, 2, "normal");
    if (total == 30.00) {
        printf("PASSED: test_calculate_order_total (normal)\n");
    } else {
        printf("FAILED: test_calculate_order_total (got %.2f, expected 30.00)\n", total);
    }
    
    total = calculate_order_total(1, 2, "1day");
    if (total == 37.50) {
        printf("PASSED: test_calculate_order_total (1day)\n");
    } else {
        printf("FAILED: test_calculate_order_total (got %.2f, expected 37.50)\n", total);
    }
}

int main(void) {
    setup();
    test_calculate_order_total();
    teardown();
    return 0;
}
