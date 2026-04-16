#ifndef ORDER_H
#define ORDER_H

#include "composition.h"
#include "customer.h"   /* will be defined later, but forward declaration is fine */

/**
 * author: Team "Kvetachki"
 * @file order.h
 * @brief Order entity – corresponds to ORDERS table in database
 */

typedef struct Order {
    int id;                     /* Primary key, auto-increment */
    int customer_id;            /* Foreign key → CUSTOMERS.id */
    int composition_id;         /* Foreign key → COMPOSITIONS.id */
    int quantity;               /* Number of compositions ordered */
    char urgency[11];           /* "normal", "1day", "2days" */
    char order_date[11];        /* YYYY-MM-DD */
    char delivery_date[11];     /* YYYY-MM-DD */
    double total_price;         /* Final price after surcharge */
    char created_at[20];        /* Timestamp */
} Order;

/* Order summary structure (from ORDER_SUMMARIES table) */
typedef struct {
    int id;
    int order_id;
    double final_price;
    char calculated_at[20];
} OrderSummary;

/* Function prototypes for orders */
int order_create(Order *o);
int order_update(Order *o);
int order_delete(int id);
Order* order_find_by_id(int id);
Order** order_find_by_customer(int customer_id, int *count);
Order** order_find_by_date(const char *from_date, const char *to_date, int *count);
void order_free(Order *o);
void order_print(const Order *o);

/* Order summary functions */
int order_summary_create(OrderSummary *os);
OrderSummary* order_summary_find_by_order_id(int order_id);
void order_summary_free(OrderSummary *os);

/* Cost calculation (business logic) */
double calculate_base_cost(int composition_id, int quantity);
double apply_urgency_surcharge(double base_cost, const char *urgency);

#endif /* ORDER_H */
