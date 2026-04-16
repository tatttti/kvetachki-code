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

/* Global database connection (used by all modules) */
sqlite3 *g_db = NULL;

/* Function prototypes */
static void show_main_menu(int user_id, const char *role);
static int login(void);
static void init_database(void);

/*============================================================
 * Main entry point
 *============================================================*/
int main(void) {
    /* Open database */
    if (db_open("greenhouse.db", &g_db) != 0) {
        fprintf(stderr, "Error: Cannot open database\n");
        return 1;
    }

    /* Initialize database tables if not exists */
    init_database();

    /* Login */
    int user_id = login();
    if (user_id <= 0) {
        printf("Login failed. Exiting...\n");
        db_close(g_db);
        return 1;
    }

    /* Get user role */
    char role[21] = {0};
    User *u = user_find_by_id(user_id);
    if (u) {
        strcpy(role, u->role);
        user_free(u);
    } else {
        strcpy(role, "MANAGER");
    }

    printf("\nWelcome! Role: %s\n", role);

    /* Show main menu based on role */
    show_main_menu(user_id, role);

    /* Close database */
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
 * Main menu (based on role)
 *============================================================*/
static void show_main_menu(int user_id, const char *role) {
    int choice;

    do {
        printf("\n========================================\n");
        printf("        FLOWER GREENHOUSE SYSTEM\n");
        printf("========================================\n");
        printf("1. Customer portal\n");
        printf("2. Order management\n");

        if (strcmp(role, "ADMIN") == 0 || strcmp(role, "MANAGER") == 0) {
            printf("3. Flower management\n");
            printf("4. Composition management\n");
            printf("5. Reports\n");
            printf("6. Price policy\n");
        }

        printf("0. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1:
                ui_customer_show_menu();
                break;
            case 2:
                ui_order_show_menu();
                break;
            case 3:
                if (strcmp(role, "ADMIN") == 0 || strcmp(role, "MANAGER") == 0) {
                    flower_management_menu();
                } else {
                    printf("Access denied.\n");
                }
                break;
            case 4:
                if (strcmp(role, "ADMIN") == 0 || strcmp(role, "MANAGER") == 0) {
                    composition_management_menu();
                } else {
                    printf("Access denied.\n");
                }
                break;
            case 5:
                if (strcmp(role, "ADMIN") == 0 || strcmp(role, "MANAGER") == 0) {
                    reports_menu();
                } else {
                    printf("Access denied.\n");
                }
                break;
            case 6:
                if (strcmp(role, "ADMIN") == 0 || strcmp(role, "MANAGER") == 0) {
                    price_policy_menu();
                } else {
                    printf("Access denied.\n");
                }
                break;
            case 0:
                printf("Goodbye!\n");
                break;
            default:
                printf("Invalid choice.\n");
        }
    } while (choice != 0);
}

/*============================================================
 * Flower management menu (called from main)
 *============================================================*/
void flower_management_menu(void) {
    int choice;
    do {
        printf("\n=== Flower Management ===\n");
        printf("1. Add new flower\n");
        printf("2. Update flower\n");
        printf("3. Delete flower\n");
        printf("4. View all flowers\n");
        printf("5. Find flower by name\n");
        printf("0. Back\n");
        printf("Choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1: {
                Flower f;
                memset(&f, 0, sizeof(Flower));
                printf("Enter flower name: ");
                fgets(f.name, sizeof(f.name), stdin);
                f.name[strcspn(f.name, "\n")] = 0;
                printf("Enter variety: ");
                fgets(f.variety, sizeof(f.variety), stdin);
                f.variety[strcspn(f.variety, "\n")] = 0;
                printf("Enter unit price: ");
                scanf("%lf", &f.unit_price);
                getchar();
                if (flower_create(&f) == 0) {
                    printf("Flower added with ID: %d\n", f.id);
                } else {
                    printf("Failed to add flower.\n");
                }
                break;
            }
            case 2: {
                int id;
                printf("Enter flower ID: ");
                scanf("%d", &id);
                getchar();
                Flower *f = flower_find_by_id(id);
                if (!f) {
                    printf("Flower not found.\n");
                    break;
                }
                printf("Current: %s (%s) - %.2f\n", f->name, f->variety, f->unit_price);

                double new_price = f->unit_price;
                printf("Enter new unit price (current: %.2f): ", f->unit_price);
                char price_buf[32];
                fgets(price_buf, sizeof(price_buf), stdin);
                if (strlen(price_buf) > 1) {
                    sscanf(price_buf, "%lf", &new_price);
                }

                /* Check price increase limit (10%) */
                if (new_price != f->unit_price) {
                    if (!can_increase_flower_price(f->id, new_price)) {
                        printf("Error: Price increase would raise composition price by more than 10%%\n");
                        printf("Operation not allowed.\n");
                        flower_free(f);
                        break;
                    }
                }
                f->unit_price = new_price;

                printf("Enter new name (or press Enter to keep): ");
                char buf[256];
                fgets(buf, sizeof(buf), stdin);
                buf[strcspn(buf, "\n")] = 0;
                if (strlen(buf) > 0) strcpy(f->name, buf);

                printf("Enter new variety: ");
                fgets(buf, sizeof(buf), stdin);
                buf[strcspn(buf, "\n")] = 0;
                if (strlen(buf) > 0) strcpy(f->variety, buf);

                if (flower_update(f) == 0) {
                    printf("Flower updated.\n");
                } else {
                    printf("Failed to update flower.\n");
                }
                flower_free(f);
                break;
            }
            case 3: {
                int id;
                printf("Enter flower ID to delete: ");
                scanf("%d", &id);
                getchar();
                if (flower_delete(id) == 0) {
                    printf("Flower deleted.\n");
                } else {
                    printf("Failed to delete flower.\n");
                }
                break;
            }
            case 4: {
                /* Simplified: would need flower_find_all */
                printf("Feature: View all flowers (implementation in progress)\n");
                break;
            }
            case 5: {
                char name[101];
                printf("Enter flower name: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;
                Flower *f = flower_find_by_name(name);
                if (f) {
                    flower_print(f);
                    flower_free(f);
                } else {
                    printf("Flower not found.\n");
                }
                break;
            }
            case 0:
                break;
            default:
                printf("Invalid choice.\n");
        }
    } while (choice != 0);
}

/*============================================================
 * Composition management menu
 *============================================================*/
void composition_management_menu(void) {
    int choice;
    do {
        printf("\n=== Composition Management ===\n");
        printf("1. Add new composition\n");
        printf("2. Update composition\n");
        printf("3. Delete composition\n");
        printf("4. View all compositions\n");
        printf("5. Add flower to composition\n");
        printf("6. View composition items\n");
        printf("0. Back\n");
        printf("Choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1: {
                Composition c;
                memset(&c, 0, sizeof(Composition));
                printf("Enter composition name: ");
                fgets(c.name, sizeof(c.name), stdin);
                c.name[strcspn(c.name, "\n")] = 0;
                printf("Enter description: ");
                fgets(c.description, sizeof(c.description), stdin);
                c.description[strcspn(c.description, "\n")] = 0;
                if (composition_create(&c) == 0) {
                    printf("Composition added with ID: %d\n", c.id);
                } else {
                    printf("Failed to add composition.\n");
                }
                break;
            }
            case 2: {
                int id;
                printf("Enter composition ID: ");
                scanf("%d", &id);
                getchar();
                Composition *c = composition_find_by_id(id);
                if (!c) {
                    printf("Composition not found.\n");
                    break;
                }
                printf("Current name: %s\n", c->name);
                printf("Enter new name (or press Enter to keep): ");
                char buf[256];
                fgets(buf, sizeof(buf), stdin);
                buf[strcspn(buf, "\n")] = 0;
                if (strlen(buf) > 0) strcpy(c->name, buf);
                printf("Enter new description: ");
                fgets(buf, sizeof(buf), stdin);
                buf[strcspn(buf, "\n")] = 0;
                if (strlen(buf) > 0) strcpy(c->description, buf);
                if (composition_update(c) == 0) {
                    printf("Composition updated.\n");
                } else {
                    printf("Failed to update composition.\n");
                }
                composition_free(c);
                break;
            }
            case 3: {
                int id;
                printf("Enter composition ID to delete: ");
                scanf("%d", &id);
                getchar();
                if (composition_delete(id) == 0) {
                    printf("Composition deleted.\n");
                } else {
                    printf("Failed to delete composition.\n");
                }
                break;
            }
            case 4: {
                int count = 0;
                Composition **comps = composition_find_all(&count);
                if (comps && count > 0) {
                    printf("\n=== All Compositions ===\n");
                    for (int i = 0; i < count; i++) {
                        composition_print(comps[i]);
                        composition_free(comps[i]);
                    }
                    free(comps);
                } else {
                    printf("No compositions found.\n");
                }
                break;
            }
            case 5: {
                int comp_id, flower_id, quantity;
                printf("Enter composition ID: ");
                scanf("%d", &comp_id);
                printf("Enter flower ID: ");
                scanf("%d", &flower_id);
                printf("Enter quantity: ");
                scanf("%d", &quantity);
                getchar();
                if (composition_add_item(comp_id, flower_id, quantity) == 0) {
                    printf("Flower added to composition.\n");
                } else {
                    printf("Failed to add flower to composition.\n");
                }
                break;
            }
            case 6: {
                int comp_id;
                printf("Enter composition ID: ");
                scanf("%d", &comp_id);
                getchar();
                int count = 0;
                CompositionItem **items = composition_get_items(comp_id, &count);
                if (items && count > 0) {
                    printf("\n=== Composition Items ===\n");
                    for (int i = 0; i < count; i++) {
                        Flower *f = flower_find_by_id(items[i]->flower_id);
                        if (f) {
                            printf("  - %s: %d pcs\n", f->name, items[i]->quantity);
                            flower_free(f);
                        }
                        free(items[i]);
                    }
                    free(items);
                } else {
                    printf("No items found for this composition.\n");
                }
                break;
            }
            case 0:
                break;
            default:
                printf("Invalid choice.\n");
        }
    } while (choice != 0);
}

/*============================================================
 * Reports menu
 *============================================================*/
void reports_menu(void) {
    int choice;
    do {
        printf("\n=== Reports ===\n");
        printf("1. Revenue for period\n");
        printf("2. Most popular composition\n");
        printf("3. Orders by urgency\n");
        printf("4. Flower usage by type\n");
        printf("5. Composition sales\n");
        printf("0. Back\n");
        printf("Choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1: {
                char from[11], to[11];
                printf("Enter from date (YYYY-MM-DD): ");
                fgets(from, sizeof(from), stdin);
                from[strcspn(from, "\n")] = 0;
                printf("Enter to date (YYYY-MM-DD): ");
                fgets(to, sizeof(to), stdin);
                to[strcspn(to, "\n")] = 0;
                double revenue = select_revenue_for_period(from, to);
                if (revenue >= 0) {
                    printf("Revenue: %.2f\n", revenue);
                } else {
                    printf("Failed to calculate revenue.\n");
                }
                break;
            }
            case 2:
                select_most_popular_composition();
                break;
            case 3:
                select_orders_by_urgency();
                break;
            case 4: {
                char from[11], to[11];
                printf("Enter from date (YYYY-MM-DD): ");
                fgets(from, sizeof(from), stdin);
                from[strcspn(from, "\n")] = 0;
                printf("Enter to date (YYYY-MM-DD): ");
                fgets(to, sizeof(to), stdin);
                to[strcspn(to, "\n")] = 0;
                select_flower_usage_by_type_and_variety(from, to);
                break;
            }
            case 5: {
                char from[11], to[11];
                printf("Enter from date (YYYY-MM-DD): ");
                fgets(from, sizeof(from), stdin);
                from[strcspn(from, "\n")] = 0;
                printf("Enter to date (YYYY-MM-DD): ");
                fgets(to, sizeof(to), stdin);
                to[strcspn(to, "\n")] = 0;
                select_composition_sales(from, to);
                break;
            }
            case 0:
                break;
            default:
                printf("Invalid choice.\n");
        }
    } while (choice != 0);
}

/*============================================================
 * Price policy menu
 *============================================================*/
void price_policy_menu(void) {
    int choice;
    do {
        printf("\n=== Price Policy ===\n");
        printf("1. View current surcharges\n");
        printf("2. Update surcharge\n");
        printf("0. Back\n");
        printf("Choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1: {
                PricePolicy **policies = price_policy_find_all(NULL);
                printf("\nCurrent surcharges:\n");
                printf("  normal: 0%%\n");
                printf("  1day: 25%%\n");
                printf("  2days: 15%%\n");
                break;
            }
            case 2:
                printf("Feature: Update surcharge (implementation in progress)\n");
                break;
            case 0:
                break;
            default:
                printf("Invalid choice.\n");
        }
    } while (choice != 0);
}

/*============================================================
 * Initialize database tables
 *============================================================*/
static void init_database(void) {
    char *err_msg = NULL;

    /* Users table */
    const char *sql_users =
    "CREATE TABLE IF NOT EXISTS USERS ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "login VARCHAR(50) NOT NULL UNIQUE,"
    "password_hash VARCHAR(128) NOT NULL,"
    "role VARCHAR(20) NOT NULL,"
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
    const char *check_policy = "SELECT COUNT(*) FROM PRICE_POLICY;";
    sqlite3_stmt *stmt;
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

    /* Insert default admin user if empty */
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

        char insert_user[512];
        snprintf(insert_user, sizeof(insert_user),
                 "INSERT INTO USERS (login, password_hash, role) VALUES "
                 "('admin', '%s', 'ADMIN');", hashed);
        db_execute(g_db, insert_user, &err_msg);
        printf("Default admin user created: login='admin', password='admin123'\n");
    }
}
