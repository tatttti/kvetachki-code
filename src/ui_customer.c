/**
 * author: Team "Kvetachki"
 * @file ui_customer.c
 * @brief User interface for customer
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "order.h"
#include "composition.h"
#include "db.h"

extern sqlite3 *g_db;

static void print_composition_list(Composition **comps, int count) {
    if (!comps || count == 0) {
        printf("No compositions found.\n");
        return;
    }
    
    printf("\n=== Available Compositions ===\n");
    for (int i = 0; i < count; i++) {
        composition_print(comps[i]);
    }
}

void ui_customer_show_menu(void) {
    int customer_id;
    printf("\n=== Customer Portal ===\n");
    printf("Enter your customer ID: ");
    scanf("%d", &customer_id);
    getchar();
    
    int choice;
    do {
        printf("\n=== Customer Menu (ID: %d) ===\n", customer_id);
        printf("1. View my orders\n");
        printf("2. View all compositions\n");
        printf("3. View composition details\n");
        printf("0. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);
        getchar();
        
        switch (choice) {
            case 1: {
                int count = 0;
                Order **orders = order_find_by_customer(customer_id, &count);
                if (!orders || count == 0) {
                    printf("You have no orders.\n");
                } else {
                    printf("\n=== Your Orders ===\n");
                    for (int i = 0; i < count; i++) {
                        order_print(orders[i]);
                    }
                    for (int i = 0; i < count; i++) order_free(orders[i]);
                    free(orders);
                }
                break;
            }
            case 2: {
                int count = 0;
                Composition **comps = composition_find_all(&count);
                print_composition_list(comps, count);
                for (int i = 0; i < count; i++) composition_free(comps[i]);
                if (comps) free(comps);
                break;
            }
            case 3: {
                int comp_id;
                printf("Enter composition ID: ");
                scanf("%d", &comp_id);
                getchar();
                Composition *c = composition_find_by_id(comp_id);
                if (c) {
                    composition_print(c);
                    composition_free(c);
                } else {
                    printf("Composition not found.\n");
                }
                break;
            }
            case 0: break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 0);
}
