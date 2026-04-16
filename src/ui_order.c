/**
 * author: Team "Kvetachki"
 * @file ui_order.c
 * @brief User interface for order management
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "order.h"
#include "order_service.h"
#include "composition.h"
#include "customer.h"
#include "db.h"

extern sqlite3 *g_db;

/* Function prototypes */
void ui_order_handle_create(void);
void ui_order_handle_update(void);
void ui_order_handle_delete(void);

static void print_order_list(Order **orders, int count) {
    if (!orders || count == 0) {
        printf("No orders found.\n");
        return;
    }
    
    printf("\n=== Orders ===\n");
    for (int i = 0; i < count; i++) {
        order_print(orders[i]);
    }
}

void ui_order_show_menu(void) {
    int choice;
    do {
        printf("\n=== Order Management ===\n");
        printf("1. Create new order\n");
        printf("2. Update order\n");
        printf("3. Delete order\n");
        printf("4. View order by ID\n");
        printf("5. View all customer orders\n");
        printf("6. View orders by date range\n");
        printf("0. Back to main menu\n");
        printf("Choice: ");
        scanf("%d", &choice);
        getchar(); /* clear newline */
        
        switch (choice) {
            case 1: ui_order_handle_create(); break;
            case 2: ui_order_handle_update(); break;
            case 3: ui_order_handle_delete(); break;
            case 4: {
                int id;
                printf("Enter order ID: ");
                scanf("%d", &id);
                getchar();
                Order *o = order_find_by_id(id);
                if (o) {
                    order_print(o);
                    order_free(o);
                } else {
                    printf("Order not found.\n");
                }
                break;
            }
            case 5: {
                int customer_id;
                printf("Enter customer ID: ");
                scanf("%d", &customer_id);
                getchar();
                int count = 0;
                Order **orders = order_find_by_customer(customer_id, &count);
                print_order_list(orders, count);
                for (int i = 0; i < count; i++) order_free(orders[i]);
                if (orders) free(orders);
                break;
            }
            case 6: {
                char from[11], to[11];
                printf("Enter from date (YYYY-MM-DD): ");
                fgets(from, sizeof(from), stdin);
                from[strcspn(from, "\n")] = 0;
                printf("Enter to date (YYYY-MM-DD): ");
                fgets(to, sizeof(to), stdin);
                to[strcspn(to, "\n")] = 0;
                int count = 0;
                Order **orders = order_find_by_date(from, to, &count);
                print_order_list(orders, count);
                for (int i = 0; i < count; i++) order_free(orders[i]);
                if (orders) free(orders);
                break;
            }
            case 0: break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 0);
}

void ui_order_handle_create(void) {
    int customer_id, composition_id, quantity;
    char urgency[11], delivery_date[11];
    
    printf("\n=== Create New Order ===\n");
    
    printf("Enter customer ID: ");
    scanf("%d", &customer_id);
    getchar();
    
    /* Display available compositions */
    int comp_count = 0;
    Composition **comps = composition_find_all(&comp_count);
    if (comps && comp_count > 0) {
        printf("\nAvailable compositions:\n");
        for (int i = 0; i < comp_count; i++) {
            composition_print(comps[i]);
        }
        for (int i = 0; i < comp_count; i++) composition_free(comps[i]);
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
}

void ui_order_handle_update(void) {
    int id;
    printf("Enter order ID to update: ");
    scanf("%d", &id);
    getchar();
    
    Order *o = order_find_by_id(id);
    if (!o) {
        printf("Order not found.\n");
        return;
    }
    
    printf("Current order details:\n");
    order_print(o);
    
    printf("\nEnter new quantity (current: %d): ", o->quantity);
    scanf("%d", &o->quantity);
    getchar();
    
    printf("Enter new urgency (current: %s): ", o->urgency);
    fgets(o->urgency, sizeof(o->urgency), stdin);
    o->urgency[strcspn(o->urgency, "\n")] = 0;
    
    printf("Enter new delivery date (current: %s): ", o->delivery_date);
    fgets(o->delivery_date, sizeof(o->delivery_date), stdin);
    o->delivery_date[strcspn(o->delivery_date, "\n")] = 0;
    
    /* Recalculate total price */
    double new_total = calculate_order_total(o->composition_id, o->quantity, o->urgency);
    if (new_total > 0) {
        o->total_price = new_total;
    }
    
    if (order_update(o) == 0) {
        printf("Order updated successfully. New total: %.2f\n", o->total_price);
    } else {
        printf("Failed to update order.\n");
    }
    
    order_free(o);
}

void ui_order_handle_delete(void) {
    int id;
    printf("Enter order ID to delete: ");
    scanf("%d", &id);
    getchar();
    
    Order *o = order_find_by_id(id);
    if (!o) {
        printf("Order not found.\n");
        return;
    }
    
    printf("Are you sure you want to delete this order?\n");
    order_print(o);
    printf("Confirm (y/n): ");
    char confirm = getchar();
    getchar();
    
    if (confirm == 'y' || confirm == 'Y') {
        if (order_delete(id) == 0) {
            printf("Order deleted successfully.\n");
        } else {
            printf("Failed to delete order.\n");
        }
    } else {
        printf("Deletion cancelled.\n");
    }
    
    order_free(o);
}
