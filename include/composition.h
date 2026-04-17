#ifndef COMPOSITION_H
#define COMPOSITION_H

#include "flower.h"

/**
 * author: Team "Kvetachki"
 * @file composition.h
 * @brief Composition entity – corresponds to COMPOSITIONS table in database
 */

typedef struct {
    int id;                     /* Primary key, auto-increment */
    char name[101];             /* Composition name (e.g., "Romantic Bouquet") */
    char description[501];      /* Detailed description */
    char created_at[20];        /* Timestamp */
} Composition;

/* Structure for composition item (flower + quantity) */
typedef struct {
    int flower_id;
    int quantity;
} CompositionItem;

/* Function prototypes */
int composition_create(Composition *c);
int composition_update(Composition *c);
int composition_delete(int id);
Composition* composition_find_by_id(int id);
Composition** composition_find_all(int *count);
void composition_free(Composition *c);
void composition_print(const Composition *c);

/* Composition items (many-to-many with flowers) */
int composition_add_item(int composition_id, int flower_id, int quantity);
int composition_remove_item(int composition_id, int flower_id);
CompositionItem** composition_get_items(int composition_id, int *count);
void composition_items_free(CompositionItem **items, int count);

#endif /* COMPOSITION_H */
