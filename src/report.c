/**
 * author: Team "Kvetachki"
 * @file report.c
 * @brief Implementation of analytics reports
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "report.h"
#include "order.h"
#include "flower.h"
#include "composition.h"
#include "db.h"

extern sqlite3 *g_db;

double select_revenue_for_period(const char *from_date, const char *to_date) {
    const char *sql = "SELECT SUM(total_price) FROM ORDERS WHERE order_date BETWEEN ? AND ?;";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Revenue query failed\n");
        return -1;
    }
    
    sqlite3_bind_text(stmt, 1, from_date, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, to_date, -1, SQLITE_STATIC);
    
    double revenue = 0.0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        revenue = sqlite3_column_double(stmt, 0);
    }
    
    sqlite3_finalize(stmt);
    return revenue;
}

void select_most_popular_composition(void) {
    const char *sql = "SELECT c.name, COUNT(o.id) as order_count "
                      "FROM COMPOSITIONS c "
                      "JOIN ORDERS o ON c.id = o.composition_id "
                      "GROUP BY c.id, c.name "
                      "ORDER BY order_count DESC LIMIT 1;";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to get popular composition\n");
        return;
    }
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *name = (const char*)sqlite3_column_text(stmt, 0);
        int count = sqlite3_column_int(stmt, 1);
        printf("Most popular composition: %s (%d orders)\n", name, count);
    } else {
        printf("No orders found\n");
    }
    
    sqlite3_finalize(stmt);
}

void select_orders_by_urgency(void) {
    const char *sql = "SELECT urgency, COUNT(*) as count, SUM(total_price) as total "
                      "FROM ORDERS GROUP BY urgency;";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to get urgency statistics\n");
        return;
    }
    
    printf("\n=== Orders by Urgency ===\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *urgency = (const char*)sqlite3_column_text(stmt, 0);
        int count = sqlite3_column_int(stmt, 1);
        double total = sqlite3_column_double(stmt, 2);
        printf("%s: %d orders, total revenue: %.2f\n", urgency, count, total);
    }
    
    sqlite3_finalize(stmt);
}

void select_flower_usage_by_type_and_variety(const char *from_date, const char *to_date) {
    const char *sql = "SELECT f.name, f.variety, SUM(ci.quantity * o.quantity) as total_used "
                      "FROM FLOWERS f "
                      "JOIN COMPOSITION_ITEMS ci ON f.id = ci.flower_id "
                      "JOIN COMPOSITIONS c ON ci.composition_id = c.id "
                      "JOIN ORDERS o ON c.id = o.composition_id "
                      "WHERE o.order_date BETWEEN ? AND ? "
                      "GROUP BY f.id, f.name, f.variety "
                      "ORDER BY total_used DESC;";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to get flower usage\n");
        return;
    }
    
    sqlite3_bind_text(stmt, 1, from_date, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, to_date, -1, SQLITE_STATIC);
    
    printf("\n=== Flower Usage (%s to %s) ===\n", from_date, to_date);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *name = (const char*)sqlite3_column_text(stmt, 0);
        const char *variety = (const char*)sqlite3_column_text(stmt, 1);
        int total = sqlite3_column_int(stmt, 2);
        printf("%s (%s): %d units\n", name, variety, total);
    }
    
    sqlite3_finalize(stmt);
}

void select_composition_sales(const char *from_date, const char *to_date) {
    const char *sql = "SELECT c.name, SUM(o.quantity) as total_sold, SUM(o.total_price) as revenue "
                      "FROM COMPOSITIONS c "
                      "JOIN ORDERS o ON c.id = o.composition_id "
                      "WHERE o.order_date BETWEEN ? AND ? "
                      "GROUP BY c.id, c.name "
                      "ORDER BY revenue DESC;";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to get composition sales\n");
        return;
    }
    
    sqlite3_bind_text(stmt, 1, from_date, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, to_date, -1, SQLITE_STATIC);
    
    printf("\n=== Composition Sales (%s to %s) ===\n", from_date, to_date);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *name = (const char*)sqlite3_column_text(stmt, 0);
        int sold = sqlite3_column_int(stmt, 1);
        double revenue = sqlite3_column_double(stmt, 2);
        printf("%s: sold %d units, revenue %.2f\n", name, sold, revenue);
    }
    
    sqlite3_finalize(stmt);
}
