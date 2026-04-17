#ifndef FLOWER_H
#define FLOWER_H

/**
 * author: Team "Kvetachki"
 * @file flower.h
 * @brief Flower entity – corresponds to FLOWERS table in database
 */

typedef struct {
    int id;                     /* Primary key, auto-increment */
    char name[101];             /* Flower name (e.g., "Rose") */
    char variety[101];          /* Flower variety (e.g., "Red") */
    double unit_price;          /* Price per single flower */
    char created_at[20];        /* Timestamp (YYYY-MM-DD HH:MM:SS) */
} Flower;

/* Function prototypes for flower operations (CRUD) */
int flower_create(Flower *f);
int flower_update(Flower *f);
int flower_delete(int id);
Flower* flower_find_by_id(int id);
Flower* flower_find_by_name(const char *name);
void flower_free(Flower *f);
void flower_print(const Flower *f);
Flower** flower_find_all(int *count);

#endif /* FLOWER_H */
