/**
 * author: Team "Kvetachki"
 * @file ui_customer.c
 * @brief User interface for customer
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "order.h"
#include "order_service.h"
#include "composition.h"
#include "flower.h"
#include "db.h"

extern sqlite3 *g_db;

void ui_customer_show_menu(int customer_id) {
    int choice;
    do {
        printf("\n=== Customer Menu (ID: %d) ===\n", customer_id);
        printf("1. View all compositions\n");
        printf("2. View composition details\n");
        printf("3. View my orders\n");
        printf("4. Create new order\n");
        printf("0. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1: {
                int count = 0;
                Composition **comps = composition_find_all(&count);
                if (comps && count > 0) {
                    printf("\n=== Available Compositions ===\n");
                    for (int i = 0; i < count; i++) {
                        composition_print(comps[i]);
                        composition_free(comps[i]);
                    }
                    free(comps);
                } else {
                    printf("No compositions found.\n");
                }
                break;
            }
            case 2: {
                int comp_id;
                printf("Enter composition ID: ");
                scanf("%d", &comp_id);
                getchar();
                Composition *c = composition_find_by_id(comp_id);
                if (c) {
                    composition_print(c);
                    int item_count = 0;
                    CompositionItem **items = composition_get_items(comp_id, &item_count);
                    if (items && item_count > 0) {
                        printf("  Composition items:\n");
                        for (int i = 0; i < item_count; i++) {
                            Flower *f = flower_find_by_id(items[i]->flower_id);
                            if (f) {
                                printf("    - %s: %d pcs\n", f->name, items[i]->quantity);
                                flower_free(f);
                            }
                            free(items[i]);
                        }
                        free(items);
                    }
                    composition_free(c);
                } else {
                    printf("Composition not found.\n");
                }
                break;
            }
            case 3: {
                int count = 0;
                Order **orders = order_find_by_customer(customer_id, &count);
                if (orders && count > 0) {
                    printf("\n=== Your Orders ===\n");
                    for (int i = 0; i < count; i++) {
                        order_print(orders[i]);
                        order_free(orders[i]);
                    }
                    free(orders);
                } else {
                    printf("You have no orders.\n");
                }
                break;
            }
            case 4: {
                int composition_id, quantity;
                char urgency[11], delivery_date[11];

                printf("\n=== Create New Order ===\n");

                int comp_count = 0;
                Composition **comps = composition_find_all(&comp_count);
                if (comps && comp_count > 0) {
                    printf("\nAvailable compositions:\n");
                    for (int i = 0; i < comp_count; i++) {
                        composition_print(comps[i]);
                        composition_free(comps[i]);
                    }
                    free(comps);
                }

                printf("Enter composition ID: ");
                scanf("%d", &composition_id);
                getchar();

                printf("Enter quantity: ");
                scanf("%d", &quantity);
                getchar();

                printf("Enter urgency (normal/1day/2days): ");
                fgets(urgency, sizeof(urgency), stdin);
                urgency[strcspn(urgency, "\n")] = 0;

                printf("Enter delivery date (YYYY-MM-DD): ");
                fgets(delivery_date, sizeof(delivery_date), stdin);
                delivery_date[strcspn(delivery_date, "\n")] = 0;

                int order_id = process_order(customer_id, composition_id, quantity,
                                             urgency, delivery_date);
                if (order_id > 0) {
                    printf("Order #%d created successfully!\n", order_id);
                } else {
                    printf("Failed to create order.\n");
                }
                break;
            }
            case 0:
                printf("Exiting customer portal.\n");
                break;
            default:
                printf("Invalid choice.\n");
        }
    } while (choice != 0);
}
