/**
 * author: Team "Kvetachki"
 * @file order_service.h
 * @brief Business logic for order processing
 */

#ifndef ORDER_SERVICE_H
#define ORDER_SERVICE_H

#include "order.h"
#include "composition.h"

/**
 * @brief Process a new order: calculate cost, apply surcharge, save to database
 * @param customer_id Customer who places the order
 * @param composition_id Composition being ordered
 * @param quantity Number of compositions
 * @param urgency Urgency type ("normal", "1day", "2days")
 * @param delivery_date Requested delivery date (YYYY-MM-DD)
 * @return Order ID on success, -1 on error
 */
int process_order(int customer_id, int composition_id, int quantity,
                  const char *urgency, const char *delivery_date);

/**
 * @brief Calculate total cost for an order (base cost + surcharge)
 * @param composition_id Composition ID
 * @param quantity Number of compositions
 * @param urgency Urgency type
 * @return Total cost, or -1 on error
 */
double calculate_order_total(int composition_id, int quantity, const char *urgency);

/**
 * @brief Validate if an order can be placed (check composition exists, etc.)
 * @param composition_id Composition ID
 * @param quantity Quantity (must be > 0)
 * @return 1 if valid, 0 if not
 */
int validate_order(int composition_id, int quantity);

/**
 * @brief Generate order summary after order is saved
 * @param order_id Order ID
 * @return 1 on success, 0 on failure
 */
int generate_order_summary(int order_id);

#endif /* ORDER_SERVICE_H */
