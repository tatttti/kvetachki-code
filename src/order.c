/**
 * author: Team "Kvetachki"
 * @file order.c
 * @brief Implementation of order CRUD operations
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "order.h"
#include "price_policy.h"
#include "db.h"

extern sqlite3 *g_db;

int order_create(Order *o) {
    if (!o || o->customer_id <= 0 || o->composition_id <= 0 || o->quantity <= 0) return -1;
    
    const char *sql = "INSERT INTO ORDERS (customer_id, composition_id, quantity, urgency, "
                      "order_date, delivery_date, total_price, created_at) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?, datetime('now'));";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;
    
    sqlite3_bind_int(stmt, 1, o->customer_id);
    sqlite3_bind_int(stmt, 2, o->composition_id);
    sqlite3_bind_int(stmt, 3, o->quantity);
    sqlite3_bind_text(stmt, 4, o->urgency, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, o->order_date, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, o->delivery_date, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 7, o->total_price);
    
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE) {
        o->id = sqlite3_last_insert_rowid(g_db);
    }
    
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? 0 : -1;
}

int order_update(Order *o) {
    if (!o || o->id <= 0) return -1;
    
    const char *sql = "UPDATE ORDERS SET customer_id = ?, composition_id = ?, quantity = ?, "
                      "urgency = ?, delivery_date = ?, total_price = ? WHERE id = ?;";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;
    
    sqlite3_bind_int(stmt, 1, o->customer_id);
    sqlite3_bind_int(stmt, 2, o->composition_id);
    sqlite3_bind_int(stmt, 3, o->quantity);
    sqlite3_bind_text(stmt, 4, o->urgency, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, o->delivery_date, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 6, o->total_price);
    sqlite3_bind_int(stmt, 7, o->id);
    
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? 0 : -1;
}

int order_delete(int id) {
    if (id <= 0) return -1;
    
    /* First delete order summary */
    const char *del_summary = "DELETE FROM ORDER_SUMMARIES WHERE order_id = ?;";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(g_db, del_summary, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
    
    /* Then delete the order */
    const char *sql = "DELETE FROM ORDERS WHERE id = ?;";
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;
    
    sqlite3_bind_int(stmt, 1, id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? 0 : -1;
}

Order* order_find_by_id(int id) {
    if (id <= 0) return NULL;
    
    const char *sql = "SELECT id, customer_id, composition_id, quantity, urgency, "
                      "order_date, delivery_date, total_price, created_at "
                      "FROM ORDERS WHERE id = ?;";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return NULL;
    
    sqlite3_bind_int(stmt, 1, id);
    
    Order *o = NULL;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        o = (Order*)malloc(sizeof(Order));
        o->id = sqlite3_column_int(stmt, 0);
        o->customer_id = sqlite3_column_int(stmt, 1);
        o->composition_id = sqlite3_column_int(stmt, 2);
        o->quantity = sqlite3_column_int(stmt, 3);
        strcpy(o->urgency, (const char*)sqlite3_column_text(stmt, 4));
        strcpy(o->order_date, (const char*)sqlite3_column_text(stmt, 5));
        strcpy(o->delivery_date, (const char*)sqlite3_column_text(stmt, 6));
        o->total_price = sqlite3_column_double(stmt, 7);
        strcpy(o->created_at, (const char*)sqlite3_column_text(stmt, 8));
    }
    
    sqlite3_finalize(stmt);
    return o;
}

Order** order_find_by_customer(int customer_id, int *count) {
    *count = 0;
    const char *sql = "SELECT id, customer_id, composition_id, quantity, urgency, "
                      "order_date, delivery_date, total_price, created_at "
                      "FROM ORDERS WHERE customer_id = ?;";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return NULL;
    
    sqlite3_bind_int(stmt, 1, customer_id);
    
    /* Count rows */
    int rows = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) rows++;
    sqlite3_reset(stmt);
    
    if (rows == 0) {
        sqlite3_finalize(stmt);
        return NULL;
    }
    
    Order **arr = (Order**)malloc(sizeof(Order*) * rows);
    int idx = 0;
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Order *o = (Order*)malloc(sizeof(Order));
        o->id = sqlite3_column_int(stmt, 0);
        o->customer_id = sqlite3_column_int(stmt, 1);
        o->composition_id = sqlite3_column_int(stmt, 2);
        o->quantity = sqlite3_column_int(stmt, 3);
        strcpy(o->urgency, (const char*)sqlite3_column_text(stmt, 4));
        strcpy(o->order_date, (const char*)sqlite3_column_text(stmt, 5));
        strcpy(o->delivery_date, (const char*)sqlite3_column_text(stmt, 6));
        o->total_price = sqlite3_column_double(stmt, 7);
        strcpy(o->created_at, (const char*)sqlite3_column_text(stmt, 8));
        arr[idx++] = o;
    }
    
    *count = idx;
    sqlite3_finalize(stmt);
    return arr;
}

Order** order_find_by_date(const char *from_date, const char *to_date, int *count) {
    *count = 0;
    const char *sql = "SELECT id, customer_id, composition_id, quantity, urgency, "
                      "order_date, delivery_date, total_price, created_at "
                      "FROM ORDERS WHERE order_date BETWEEN ? AND ?;";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return NULL;
    
    sqlite3_bind_text(stmt, 1, from_date, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, to_date, -1, SQLITE_STATIC);
    
    int rows = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) rows++;
    sqlite3_reset(stmt);
    
    if (rows == 0) {
        sqlite3_finalize(stmt);
        return NULL;
    }
    
    Order **arr = (Order**)malloc(sizeof(Order*) * rows);
    int idx = 0;
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Order *o = (Order*)malloc(sizeof(Order));
        o->id = sqlite3_column_int(stmt, 0);
        o->customer_id = sqlite3_column_int(stmt, 1);
        o->composition_id = sqlite3_column_int(stmt, 2);
        o->quantity = sqlite3_column_int(stmt, 3);
        strcpy(o->urgency, (const char*)sqlite3_column_text(stmt, 4));
        strcpy(o->order_date, (const char*)sqlite3_column_text(stmt, 5));
        strcpy(o->delivery_date, (const char*)sqlite3_column_text(stmt, 6));
        o->total_price = sqlite3_column_double(stmt, 7);
        strcpy(o->created_at, (const char*)sqlite3_column_text(stmt, 8));
        arr[idx++] = o;
    }
    
    *count = idx;
    sqlite3_finalize(stmt);
    return arr;
}

void order_free(Order *o) {
    if (o) free(o);
}

void order_print(const Order *o) {
    if (!o) return;
    printf("Order ID: %d | Customer: %d | Composition: %d | Qty: %d | Urgency: %s | "
           "Order Date: %s | Delivery: %s | Total: %.2f\n",
           o->id, o->customer_id, o->composition_id, o->quantity, o->urgency,
           o->order_date, o->delivery_date, o->total_price);
}

int order_summary_create(OrderSummary *os) {
    if (!os || os->order_id <= 0) return -1;
    
    const char *sql = "INSERT INTO ORDER_SUMMARIES (order_id, final_price, calculated_at) "
                      "VALUES (?, ?, datetime('now'));";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;
    
    sqlite3_bind_int(stmt, 1, os->order_id);
    sqlite3_bind_double(stmt, 2, os->final_price);
    
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE) {
        os->id = sqlite3_last_insert_rowid(g_db);
    }
    
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? 0 : -1;
}

OrderSummary* order_summary_find_by_order_id(int order_id) {
    const char *sql = "SELECT id, order_id, final_price, calculated_at FROM ORDER_SUMMARIES WHERE order_id = ?;";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return NULL;
    
    sqlite3_bind_int(stmt, 1, order_id);
    
    OrderSummary *os = NULL;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        os = (OrderSummary*)malloc(sizeof(OrderSummary));
        os->id = sqlite3_column_int(stmt, 0);
        os->order_id = sqlite3_column_int(stmt, 1);
        os->final_price = sqlite3_column_double(stmt, 2);
        strcpy(os->calculated_at, (const char*)sqlite3_column_text(stmt, 3));
    }
    
    sqlite3_finalize(stmt);
    return os;
}

void order_summary_free(OrderSummary *os) {
    if (os) free(os);
}

double calculate_base_cost(int composition_id, int quantity) {
    /* Simplified: get composition items from DB and sum flower prices */
    int count = 0;
    CompositionItem **items = composition_get_items(composition_id, &count);
    if (!items || count == 0) return -1;
    
    double total = 0.0;
    for (int i = 0; i < count; i++) {
        Flower *f = flower_find_by_id(items[i]->flower_id);
        if (f) {
            total += f->unit_price * items[i]->quantity;
            flower_free(f);
        }
    }
    
    composition_items_free(items, count);
    return total * quantity;
}

double apply_urgency_surcharge(double base_cost, const char *urgency) {
    double surcharge = get_surcharge_by_urgency(urgency);
    return base_cost * (1.0 + surcharge / 100.0);
}
