/**
 * author: Team "Kvetachki"
 * @file main.c
 * @brief Main entry point for Flower Greenhouse System
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

#include "db.h"
#include "user.h"
#include "flower.h"
#include "composition.h"
#include "order.h"
#include "order_service.h"
#include "report.h"
#include "price_policy.h"
#include "customer.h"

/* External UI functions (implemented in ui_*.c files) */
extern void ui_customer_show_menu(int customer_id);
extern void ui_order_show_menu(void);
extern void ui_manager_show_menu(void);

/* Global database connection */
sqlite3 *g_db = NULL;

/* Function prototypes */
static int login(void);
static void init_database(void);

/*============================================================
 * Main entry point
 *============================================================*/
int main(void) {
    if (db_open("greenhouse.db", &g_db) != 0) {
        fprintf(stderr, "Error: Cannot open database\n");
        return 1;
    }

    init_database();

    int user_id = login();
    if (user_id <= 0) {
        printf("Login failed. Exiting...\n");
        db_close(g_db);
        return 1;
    }

    char role[21] = {0};
    User *u = user_find_by_id(user_id);
    if (u) {
        strcpy(role, u->role);
        user_free(u);
    } else {
        strcpy(role, "MANAGER");
    }

    printf("\nWelcome! Role: %s\n", role);

    /* Show appropriate menu based on role */
    if (strcmp(role, "CUSTOMER") == 0) {
        int customer_id = user_get_customer_id(user_id);
        ui_customer_show_menu(customer_id);
    } else {
        ui_manager_show_menu();
    }

    db_close(g_db);
    return 0;
}

/*============================================================
 * Login function
 *============================================================*/
static int login(void) {
    char login[51];
    char password[129];

    printf("\n=== Flower Greenhouse System ===\n");
    printf("Login: ");
    fgets(login, sizeof(login), stdin);
    login[strcspn(login, "\n")] = 0;

    printf("Password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0;

    int user_id = user_authenticate(login, password);

    if (user_id > 0) {
        printf("Login successful!\n");
    } else {
        printf("Invalid login or password.\n");
    }

    return user_id;
}

/*============================================================
 * Initialize database tables
 *============================================================*/
static void init_database(void) {
    char *err_msg = NULL;

    /* Users table (with customer_id field) */
    const char *sql_users =
    "CREATE TABLE IF NOT EXISTS USERS ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "login VARCHAR(50) NOT NULL UNIQUE,"
    "password_hash VARCHAR(128) NOT NULL,"
    "role VARCHAR(20) NOT NULL,"
    "customer_id INTEGER,"
    "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
    ");";

    /* Flowers table */
    const char *sql_flowers =
    "CREATE TABLE IF NOT EXISTS FLOWERS ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "name VARCHAR(100) NOT NULL,"
    "variety VARCHAR(100),"
    "unit_price DECIMAL(10,2) NOT NULL,"
    "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
    ");";

    /* Compositions table */
    const char *sql_compositions =
    "CREATE TABLE IF NOT EXISTS COMPOSITIONS ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "name VARCHAR(100) NOT NULL,"
    "description TEXT,"
    "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
    ");";

    /* Composition items table */
    const char *sql_composition_items =
    "CREATE TABLE IF NOT EXISTS COMPOSITION_ITEMS ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "composition_id INTEGER NOT NULL,"
    "flower_id INTEGER NOT NULL,"
    "quantity INTEGER NOT NULL,"
    "FOREIGN KEY (composition_id) REFERENCES COMPOSITIONS(id),"
    "FOREIGN KEY (flower_id) REFERENCES FLOWERS(id)"
    ");";

    /* Customers table */
    const char *sql_customers =
    "CREATE TABLE IF NOT EXISTS CUSTOMERS ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "name VARCHAR(100) NOT NULL,"
    "phone VARCHAR(20),"
    "email VARCHAR(100),"
    "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
    ");";

    /* Orders table */
    const char *sql_orders =
    "CREATE TABLE IF NOT EXISTS ORDERS ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "customer_id INTEGER NOT NULL,"
    "composition_id INTEGER NOT NULL,"
    "quantity INTEGER NOT NULL,"
    "urgency VARCHAR(10) NOT NULL,"
    "order_date DATE NOT NULL,"
    "delivery_date DATE,"
    "total_price DECIMAL(10,2) NOT NULL,"
    "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
    "FOREIGN KEY (customer_id) REFERENCES CUSTOMERS(id),"
    "FOREIGN KEY (composition_id) REFERENCES COMPOSITIONS(id)"
    ");";

    /* Order summaries table */
    const char *sql_order_summaries =
    "CREATE TABLE IF NOT EXISTS ORDER_SUMMARIES ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "order_id INTEGER NOT NULL,"
    "final_price DECIMAL(10,2) NOT NULL,"
    "calculated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
    "FOREIGN KEY (order_id) REFERENCES ORDERS(id)"
    ");";

    /* Price policy table */
    const char *sql_price_policy =
    "CREATE TABLE IF NOT EXISTS PRICE_POLICY ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "urgency_type VARCHAR(10) NOT NULL UNIQUE,"
    "surcharge_percent DECIMAL(5,2) NOT NULL,"
    "description VARCHAR(255),"
    "updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
    ");";

    /* Execute all CREATE TABLE statements */
    db_execute(g_db, sql_users, &err_msg);
    db_execute(g_db, sql_flowers, &err_msg);
    db_execute(g_db, sql_compositions, &err_msg);
    db_execute(g_db, sql_composition_items, &err_msg);
    db_execute(g_db, sql_customers, &err_msg);
    db_execute(g_db, sql_orders, &err_msg);
    db_execute(g_db, sql_order_summaries, &err_msg);
    db_execute(g_db, sql_price_policy, &err_msg);

    /* Insert default price policy if empty */
    sqlite3_stmt *stmt;
    const char *check_policy = "SELECT COUNT(*) FROM PRICE_POLICY;";
    sqlite3_prepare_v2(g_db, check_policy, -1, &stmt, NULL);
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);

    if (count == 0) {
        const char *insert_policy =
        "INSERT INTO PRICE_POLICY (urgency_type, surcharge_percent, description) VALUES "
        "('normal', 0, 'Normal delivery – no surcharge'),"
        "('1day', 25, '1 day delivery – 25%% surcharge'),"
        "('2days', 15, '2 days delivery – 15%% surcharge');";
        db_execute(g_db, insert_policy, &err_msg);
    }

    /* Insert default users and customer if empty */
    const char *check_user = "SELECT COUNT(*) FROM USERS;";
    sqlite3_prepare_v2(g_db, check_user, -1, &stmt, NULL);
    count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);

    if (count == 0) {
        char hashed[129];
        hash_password("admin123", hashed, sizeof(hashed));

        /* Create default customer first */
        const char *insert_customer =
        "INSERT INTO CUSTOMERS (name, phone, email) VALUES "
        "('Default Customer', '', '');";
        db_execute(g_db, insert_customer, &err_msg);

        /* Get the customer ID */
        int customer_id = sqlite3_last_insert_rowid(g_db);

        /* Create users with customer_id for customer role */
        char insert_user[1024];
        snprintf(insert_user, sizeof(insert_user),
                 "INSERT INTO USERS (login, password_hash, role, customer_id) VALUES "
                 "('admin', '%s', 'ADMIN', NULL),"
                 "('customer', '%s', 'CUSTOMER', %d);", hashed, hashed, customer_id);
        db_execute(g_db, insert_user, &err_msg);

        printf("Default users created:\n");
        printf("  Admin: login='admin', password='admin123'\n");
        printf("  Customer: login='customer', password='admin123' (customer_id=%d)\n", customer_id);
    }
}
