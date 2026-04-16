/**
 * author: Team "Kvetachki"
 * @file customer.h
 * @brief Customer entity
 */

#ifndef CUSTOMER_H
#define CUSTOMER_H

typedef struct {
    int id;
    char name[101];
    char phone[21];
    char email[101];
    char created_at[20];
} Customer;

/* CRUD operations */
int customer_create(Customer *c);
int customer_update(Customer *c);
int customer_delete(int id);
Customer* customer_find_by_id(int id);
Customer** customer_find_all(int *count);
void customer_free(Customer *c);
void customer_print(const Customer *c);

#endif /* CUSTOMER_H */
