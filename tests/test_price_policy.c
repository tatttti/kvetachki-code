/**
 * @file test_price_policy.c
 * @brief Unit tests for price policy module
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "../include/price_policy.h"
#include "../include/db.h"

sqlite3 *g_db = NULL;

extern sqlite3 *g_db;

static void setup(void) {
    sqlite3_open(":memory:", &g_db);
    const char *sql = "CREATE TABLE PRICE_POLICY ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "urgency_type VARCHAR(10) NOT NULL UNIQUE,"
                      "surcharge_percent DECIMAL(5,2) NOT NULL,"
                      "description VARCHAR(255),"
                      "updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP);";
    char *err = NULL;
    sqlite3_exec(g_db, sql, NULL, NULL, &err);
    
    sqlite3_exec(g_db, "INSERT INTO PRICE_POLICY (urgency_type, surcharge_percent, description) VALUES "
                      "('normal', 0, 'Normal delivery'),"
                      "('1day', 25, '1 day delivery'),"
                      "('2days', 15, '2 days delivery');", NULL, NULL, &err);
}

static void teardown(void) {
    sqlite3_close(g_db);
    g_db = NULL;
}

static void test_price_policy_find_by_urgency(void) {
    PricePolicy *pp = price_policy_find_by_urgency("1day");
    if (pp && pp->surcharge_percent == 25.0) {
        printf("PASSED: test_price_policy_find_by_urgency\n");
        price_policy_free(pp);
    } else {
        printf("FAILED: test_price_policy_find_by_urgency\n");
    }
}

static void test_get_surcharge_by_urgency(void) {
    double surcharge = get_surcharge_by_urgency("2days");
    if (surcharge == 15.0) {
        printf("PASSED: test_get_surcharge_by_urgency (%.2f)\n", surcharge);
    } else {
        printf("FAILED: test_get_surcharge_by_urgency (got %.2f, expected 15.0)\n", surcharge);
    }
}

static void test_price_policy_update_surcharge(void) {
    int rc = price_policy_update_surcharge("1day", 30.0);
    PricePolicy *pp = price_policy_find_by_urgency("1day");
    
    if (rc == 0 && pp && pp->surcharge_percent == 30.0) {
        printf("PASSED: test_price_policy_update_surcharge\n");
        price_policy_free(pp);
    } else {
        printf("FAILED: test_price_policy_update_surcharge\n");
    }
}

int main(void) {
    setup();
    printf("\n=== Price Policy Module Tests ===\n");
    test_price_policy_find_by_urgency();
    test_get_surcharge_by_urgency();
    test_price_policy_update_surcharge();
    teardown();
    return 0;
}
