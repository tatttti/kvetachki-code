/**
 * author: Team "Kvetachki"
 * @file customer.h
 * @brief Customer entity – corresponds to CUSTOMERS table in database
 */

#ifndef CUSTOMER_H
#define CUSTOMER_H

/**
 * @struct Customer
 * @brief Customer information (person who places orders)
 */
typedef struct {
    int id;                     /* Primary key, auto-increment */
    char name[101];             /* Customer full name */
    char phone[21];             /* Contact phone number */
    char email[101];            /* Email address */
    char created_at[20];        /* Registration timestamp (YYYY-MM-DD HH:MM:SS) */
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
