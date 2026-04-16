/**
 * author: Team "Kvetachki"
 * @file price_policy.h
 * @brief Price policy entity – corresponds to PRICE_POLICY table in database
 */

#ifndef PRICE_POLICY_H
#define PRICE_POLICY_H

/**
 * @struct PricePolicy
 * @brief Configuration for urgency surcharges
 */
typedef struct {
    int id;                     /* Primary key, auto-increment */
    char urgency_type[11];      /* "normal", "1day", "2days" */
    double surcharge_percent;   /* 0, 15, 25 percent */
    char description[256];      /* Human-readable description */
    char updated_at[20];        /* Last update timestamp */
} PricePolicy;

/* CRUD operations */
int price_policy_create(PricePolicy *pp);
int price_policy_update(PricePolicy *pp);
int price_policy_delete(int id);
PricePolicy* price_policy_find_by_id(int id);
PricePolicy* price_policy_find_by_urgency(const char *urgency_type);
PricePolicy** price_policy_find_all(int *count);
void price_policy_free(PricePolicy *pp);
void price_policy_print(const PricePolicy *pp);

/* Helper function to get surcharge percentage directly */
double get_surcharge_by_urgency(const char *urgency_type);

#endif /* PRICE_POLICY_H */
