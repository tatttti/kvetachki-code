/**
 * author: Team "Kvetachki"
 * @file order.h
 * @brief Order entity – corresponds to ORDERS table in database
 */

#ifndef ORDER_H
#define ORDER_H

#include "composition.h"

/* Forward declaration – customer.h не існуе, але customer_id нам патрэбны */
/* Замест customer.h проста выкарыстоўваем int customer_id */

typedef struct Order {
    int id;
    int customer_id;            /* Foreign key to CUSTOMERS table */
    int composition_id;
    int quantity;
    char urgency[11];
    char order_date[11];
    char delivery_date[11];
    double total_price;
    char created_at[20];
} Order;

typedef struct {
    int id;
    int order_id;
    double final_price;
    char calculated_at[20];
} OrderSummary;

/* Function prototypes */
int order_create(Order *o);
int order_update(Order *o);
int order_delete(int id);
Order* order_find_by_id(int id);
Order** order_find_by_customer(int customer_id, int *count);
Order** order_find_by_date(const char *from_date, const char *to_date, int *count);
void order_free(Order *o);
void order_print(const Order *o);

int order_summary_create(OrderSummary *os);
OrderSummary* order_summary_find_by_order_id(int order_id);
void order_summary_free(OrderSummary *os);

double calculate_base_cost(int composition_id, int quantity);
double apply_urgency_surcharge(double base_cost, const char *urgency);

#endif /* ORDER_H */
