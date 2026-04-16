/**
 * author: Team "Kvetachki"
 * @file user.h
 * @brief User entity for authentication and authorization
 */

#ifndef USER_H
#define USER_H

/**
 * @struct User
 * @brief System user (manager/admin)
 */
typedef struct {
    int id;
    char login[51];
    char password_hash[129];
    char role[21];          /* "ADMIN", "MANAGER" */
    char created_at[20];
} User;

/* Authentication functions */
int user_authenticate(const char *login, const char *password);
int user_get_role(const char *login, char *role, int size);
int user_get_id_by_login(const char *login);

/* CRUD operations for users */
int user_create(User *u);
int user_update(User *u);
int user_delete(int id);
User* user_find_by_id(int id);
User* user_find_by_login(const char *login);
void user_free(User *u);
void user_print(const User *u);

/* Password hashing (simple hash for demo) */
void hash_password(const char *password, char *output, int size);
int user_get_customer_id(int user_id);

#endif /* USER_H */
