/**
 * author: Team "Kvetachki"
 * @file order_service.c
 * @brief Business logic for order processing
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "order_service.h"
#include "order.h"
#include "composition.h"
#include "price_policy.h"
#include "db.h"

extern sqlite3 *g_db;

int validate_order(int composition_id, int quantity) {
    if (composition_id <= 0 || quantity <= 0) return 0;
    
    Composition *c = composition_find_by_id(composition_id);
    if (!c) return 0;
    
    composition_free(c);
    return 1;
}

double calculate_order_total(int composition_id, int quantity, const char *urgency) {
    double base_cost = calculate_base_cost(composition_id, quantity);
    if (base_cost < 0) return -1;
    
    double total = apply_urgency_surcharge(base_cost, urgency);
    return total;
}

int generate_order_summary(int order_id) {
    Order *o = order_find_by_id(order_id);
    if (!o) return 0;
    
    OrderSummary os;
    os.order_id = order_id;
    os.final_price = o->total_price;
    
    int rc = order_summary_create(&os);
    order_free(o);
    return (rc == 0) ? 1 : 0;
}

int process_order(int customer_id, int composition_id, int quantity,
                  const char *urgency, const char *delivery_date) {
    /* Step 1: Validate order */
    if (!validate_order(composition_id, quantity)) {
        fprintf(stderr, "Error: Invalid order (composition_id=%d, quantity=%d)\n", 
                composition_id, quantity);
        return -1;
    }
    
    /* Step 2: Calculate total cost */
    double total = calculate_order_total(composition_id, quantity, urgency);
    if (total < 0) {
        fprintf(stderr, "Error: Failed to calculate order total\n");
        return -1;
    }
    
    /* Step 3: Prepare order object */
    Order o;
    o.id = 0;
    o.customer_id = customer_id;
    o.composition_id = composition_id;
    o.quantity = quantity;
    strcpy(o.urgency, urgency);
    
    /* Get current date for order_date */
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(o.order_date, sizeof(o.order_date), "%Y-%m-%d", t);
    
    strcpy(o.delivery_date, delivery_date);
    o.total_price = total;
    
    /* Step 4: Save order to database */
    if (order_create(&o) != 0) {
        fprintf(stderr, "Error: Failed to save order to database\n");
        return -1;
    }
    
    /* Step 5: Generate order summary */
    if (!generate_order_summary(o.id)) {
        fprintf(stderr, "Warning: Failed to generate order summary for order %d\n", o.id);
        /* Not a fatal error, order was saved */
    }
    
    printf("Order #%d successfully created! Total: %.2f\n", o.id, total);
    return o.id;
}
