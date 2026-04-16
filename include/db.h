/**
 * author: Team "Kvetachki"
 * @file db.h
 * @brief Database access layer for SQLite
 */

#ifndef DB_H
#define DB_H

#include <sqlite3.h>
#include "flower.h"
#include "composition.h"
#include "customer.h"
#include "order.h"
#include "price_policy.h"

/* Database connection functions */
int db_open(const char *filename, sqlite3 **db);
void db_close(sqlite3 *db);
int db_execute(sqlite3 *db, const char *sql, char **err_msg);

/* Flower CRUD */
int db_flower_insert(Flower *f);
int db_flower_update(Flower *f);
int db_flower_delete(int id);
Flower* db_flower_select_by_id(int id);
Flower** db_flower_select_all(int *count);

/* Composition CRUD */
int db_composition_insert(Composition *c);
int db_composition_update(Composition *c);
int db_composition_delete(int id);
Composition* db_composition_select_by_id(int id);
Composition** db_composition_select_all(int *count);

/* Composition Items (many-to-many) */
int db_composition_item_insert(int composition_id, int flower_id, int quantity);
int db_composition_item_delete(int composition_id, int flower_id);
CompositionItem** db_composition_items_select(int composition_id, int *count);

/* Customer CRUD */
int db_customer_insert(Customer *c);
int db_customer_update(Customer *c);
int db_customer_delete(int id);
Customer* db_customer_select_by_id(int id);
Customer** db_customer_select_all(int *count);

/* Order CRUD */
int db_order_insert(Order *o);
int db_order_update(Order *o);
int db_order_delete(int id);
Order* db_order_select_by_id(int id);
Order** db_order_select_by_customer(int customer_id, int *count);
Order** db_order_select_by_date_range(const char *from, const char *to, int *count);

/* Order Summary CRUD */
int db_order_summary_insert(OrderSummary *os);
OrderSummary* db_order_summary_select_by_order_id(int order_id);

/* Price Policy CRUD */
int db_price_policy_insert(PricePolicy *pp);
int db_price_policy_update(PricePolicy *pp);
PricePolicy* db_price_policy_select_by_urgency(const char *urgency_type);

/* User CRUD (add to existing db.h) */
int db_user_insert(User *u);
int db_user_update(User *u);
int db_user_delete(int id);
User* db_user_select_by_id(int id);
User* db_user_select_by_login(const char *login);

#endif /* DB_H */
