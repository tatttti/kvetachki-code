/**
 * author: Team "Kvetachki"
 * @file ui_manager.c
 * @brief User interface for manager (reports and administration)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "flower.h"
#include "composition.h"
#include "report.h"
#include "price_policy.h"
#include "db.h"

extern sqlite3 *g_db;

void flower_management_menu(void) {
    int choice;
    do {
        printf("\n=== Flower Management ===\n");
        printf("1. Add new flower\n");
        printf("2. Update flower\n");
        printf("3. Delete flower\n");
        printf("4. View all flowers\n");
        printf("5. Find flower by ID\n");
        printf("6. Find flower by name\n");
        printf("0. Back\n");
        printf("Choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1: {
                Flower f;
                memset(&f, 0, sizeof(Flower));
                printf("Enter flower name: ");
                fgets(f.name, sizeof(f.name), stdin);
                f.name[strcspn(f.name, "\n")] = 0;
                printf("Enter variety: ");
                fgets(f.variety, sizeof(f.variety), stdin);
                f.variety[strcspn(f.variety, "\n")] = 0;
                printf("Enter unit price: ");
                scanf("%lf", &f.unit_price);
                getchar();
                if (flower_create(&f) == 0) {
                    printf("Flower added with ID: %d\n", f.id);
                } else {
                    printf("Failed to add flower.\n");
                }
                break;
            }
            case 2: {
                int id;
                printf("Enter flower ID: ");
                scanf("%d", &id);
                getchar();
                Flower *f = flower_find_by_id(id);
                if (!f) {
                    printf("Flower not found.\n");
                    break;
                }
                printf("Current: %s (%s) - %.2f\n", f->name, f->variety, f->unit_price);

                double new_price = f->unit_price;
                printf("Enter new unit price (current: %.2f): ", f->unit_price);
                char price_buf[32];
                fgets(price_buf, sizeof(price_buf), stdin);
                if (strlen(price_buf) > 1) {
                    sscanf(price_buf, "%lf", &new_price);
                }

                /* Check price increase limit (10%) */
                if (new_price != f->unit_price) {
                    if (!can_increase_flower_price(f->id, new_price)) {
                        printf("Error: Price increase would raise composition price by more than 10%%\n");
                        printf("Operation not allowed.\n");
                        flower_free(f);
                        break;
                    }
                }
                f->unit_price = new_price;

                printf("Enter new name (or press Enter to keep): ");
                char buf[256];
                fgets(buf, sizeof(buf), stdin);
                buf[strcspn(buf, "\n")] = 0;
                if (strlen(buf) > 0) strcpy(f->name, buf);

                printf("Enter new variety: ");
                fgets(buf, sizeof(buf), stdin);
                buf[strcspn(buf, "\n")] = 0;
                if (strlen(buf) > 0) strcpy(f->variety, buf);

                if (flower_update(f) == 0) {
                    printf("Flower updated.\n");
                } else {
                    printf("Failed to update flower.\n");
                }
                flower_free(f);
                break;
            }
            case 3: {
                int id;
                printf("Enter flower ID to delete: ");
                scanf("%d", &id);
                getchar();
                if (flower_delete(id) == 0) {
                    printf("Flower deleted.\n");
                } else {
                    printf("Failed to delete flower.\n");
                }
                break;
            }
            case 4: {
                int count = 0;
                Flower **flowers = flower_find_all(&count);
                if (!flowers || count == 0) {
                    printf("No flowers found.\n");
                } else {
                    printf("\n=== All Flowers ===\n");
                    for (int i = 0; i < count; i++) {
                        flower_print(flowers[i]);
                        flower_free(flowers[i]);
                    }
                    free(flowers);
                }
                break;
            }
            case 5: {
                int id;
                printf("Enter flower ID: ");
                scanf("%d", &id);
                getchar();
                Flower *f = flower_find_by_id(id);
                if (f) {
                    flower_print(f);
                    flower_free(f);
                } else {
                    printf("Flower not found.\n");
                }
                break;
            }
            case 6: {
                char name[101];
                printf("Enter flower name: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;
                Flower *f = flower_find_by_name(name);
                if (f) {
                    flower_print(f);
                    flower_free(f);
                } else {
                    printf("Flower not found.\n");
                }
                break;
            }
            case 0:
                break;
            default:
                printf("Invalid choice.\n");
        }
    } while (choice != 0);
}

static void composition_management_menu(void) {
    int choice;
    do {
        printf("\n=== Composition Management ===\n");
        printf("1. Add new composition\n");
        printf("2. Update composition\n");
        printf("3. Delete composition\n");
        printf("4. View all compositions\n");
        printf("5. Add flower to composition\n");
        printf("6. View composition items\n");
        printf("0. Back\n");
        printf("Choice: ");
        scanf("%d", &choice);
        getchar();
        
        switch (choice) {
            case 1: {
                Composition c;
                memset(&c, 0, sizeof(Composition));
                printf("Enter composition name: ");
                fgets(c.name, sizeof(c.name), stdin);
                c.name[strcspn(c.name, "\n")] = 0;
                printf("Enter description: ");
                fgets(c.description, sizeof(c.description), stdin);
                c.description[strcspn(c.description, "\n")] = 0;
                if (composition_create(&c) == 0) {
                    printf("Composition added with ID: %d\n", c.id);
                } else {
                    printf("Failed to add composition.\n");
                }
                break;
            }
            case 2: {
                int id;
                printf("Enter composition ID: ");
                scanf("%d", &id);
                getchar();
                Composition *c = composition_find_by_id(id);
                if (!c) {
                    printf("Composition not found.\n");
                    break;
                }
                printf("Current name: %s\n", c->name);
                printf("Enter new name (or press Enter to keep): ");
                char buf[256];
                fgets(buf, sizeof(buf), stdin);
                buf[strcspn(buf, "\n")] = 0;
                if (strlen(buf) > 0) strcpy(c->name, buf);
                printf("Enter new description: ");
                fgets(buf, sizeof(buf), stdin);
                buf[strcspn(buf, "\n")] = 0;
                if (strlen(buf) > 0) strcpy(c->description, buf);
                if (composition_update(c) == 0) {
                    printf("Composition updated.\n");
                } else {
                    printf("Failed to update composition.\n");
                }
                composition_free(c);
                break;
            }
            case 3: {
                int id;
                printf("Enter composition ID to delete: ");
                scanf("%d", &id);
                getchar();
                if (composition_delete(id) == 0) {
                    printf("Composition deleted.\n");
                } else {
                    printf("Failed to delete composition.\n");
                }
                break;
            }
            case 4: {
                int count = 0;
                Composition **comps = composition_find_all(&count);
                if (comps && count > 0) {
                    printf("\n=== All Compositions ===\n");
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
            case 5: {
                int comp_id, flower_id, quantity;
                printf("Enter composition ID: ");
                scanf("%d", &comp_id);
                printf("Enter flower ID: ");
                scanf("%d", &flower_id);
                printf("Enter quantity: ");
                scanf("%d", &quantity);
                getchar();
                if (composition_add_item(comp_id, flower_id, quantity) == 0) {
                    printf("Flower added to composition.\n");
                } else {
                    printf("Failed to add flower to composition.\n");
                }
                break;
            }
            case 6: {
                int comp_id;
                printf("Enter composition ID: ");
                scanf("%d", &comp_id);
                getchar();
                int count = 0;
                CompositionItem **items = composition_get_items(comp_id, &count);
                if (items && count > 0) {
                    printf("\n=== Composition Items ===\n");
                    for (int i = 0; i < count; i++) {
                        Flower *f = flower_find_by_id(items[i]->flower_id);
                        if (f) {
                            printf("  - %s: %d pcs\n", f->name, items[i]->quantity);
                            flower_free(f);
                        }
                        free(items[i]);
                    }
                    free(items);
                } else {
                    printf("No items found for this composition.\n");
                }
                break;
            }
            case 0: break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 0);
}

void price_policy_menu(void) {
    int choice;
    do {
        printf("\n=== Price Policy ===\n");
        printf("1. View current surcharges\n");
        printf("2. Update surcharge\n");
        printf("0. Back\n");
        printf("Choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1: {
                printf("\nCurrent surcharges:\n");
                printf("  normal: 0%%\n");
                printf("  1day: 25%%\n");
                printf("  2days: 15%%\n");
                break;
            }
            case 2: {
                int sub_choice;
                printf("\n=== Update Surcharge ===\n");
                printf("1. Normal (currently 0%%)\n");
                printf("2. 1 day (currently 25%%)\n");
                printf("3. 2 days (currently 15%%)\n");
                printf("0. Back\n");
                printf("Choice: ");
                scanf("%d", &sub_choice);
                getchar();

                double new_surcharge;
                const char *type = NULL;

                switch (sub_choice) {
                    case 1:
                        type = "normal";
                        printf("Enter new surcharge percentage for normal delivery (current 0): ");
                        scanf("%lf", &new_surcharge);
                        getchar();
                        break;
                    case 2:
                        type = "1day";
                        printf("Enter new surcharge percentage for 1-day delivery (current 25): ");
                        scanf("%lf", &new_surcharge);
                        getchar();
                        break;
                    case 3:
                        type = "2days";
                        printf("Enter new surcharge percentage for 2-day delivery (current 15): ");
                        scanf("%lf", &new_surcharge);
                        getchar();
                        break;
                    case 0:
                        break;
                    default:
                        printf("Invalid choice.\n");
                        break;
                }

                if (type != NULL) {
                    if (price_policy_update_surcharge(type, new_surcharge) == 0) {
                        printf("Surcharge for '%s' updated to %.2f%%\n", type, new_surcharge);
                    } else {
                        printf("Failed to update surcharge.\n");
                    }
                }
                break;
            }
            case 0:
                break;
            default:
                printf("Invalid choice.\n");
        }
    } while (choice != 0);
}


void ui_manager_show_menu(void) {
    int choice;
    do {
        printf("\n=== Manager Menu ===\n");
        printf("1. Reports\n");
        printf("2. Flower management\n");
        printf("3. Composition management\n");
        printf("4. Price policy settings\n");
        printf("0. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);
        getchar();
        
        switch (choice) {
            case 1: {
                int report_choice;
                do {
                    printf("\n=== Reports ===\n");
                    printf("1. Revenue for period\n");
                    printf("2. Most popular composition\n");
                    printf("3. Orders by urgency\n");
                    printf("4. Flower usage by type\n");
                    printf("5. Composition sales\n");
                    printf("0. Back\n");
                    printf("Choice: ");
                    scanf("%d", &report_choice);
                    getchar();
                    
                    switch (report_choice) {
                        case 1: {
                            char from[11], to[11];
                            printf("Enter from date (YYYY-MM-DD): ");
                            fgets(from, sizeof(from), stdin);
                            from[strcspn(from, "\n")] = 0;
                            printf("Enter to date (YYYY-MM-DD): ");
                            fgets(to, sizeof(to), stdin);
                            to[strcspn(to, "\n")] = 0;
                            double revenue = select_revenue_for_period(from, to);
                            if (revenue >= 0) {
                                printf("Revenue: %.2f\n", revenue);
                            } else {
                                printf("Failed to calculate revenue.\n");
                            }
                            break;
                        }
                        case 2:
                            select_most_popular_composition();
                            break;
                        case 3:
                            select_orders_by_urgency();
                            break;
                        case 4: {
                            char from[11], to[11];
                            printf("Enter from date (YYYY-MM-DD): ");
                            fgets(from, sizeof(from), stdin);
                            from[strcspn(from, "\n")] = 0;
                            printf("Enter to date (YYYY-MM-DD): ");
                            fgets(to, sizeof(to), stdin);
                            to[strcspn(to, "\n")] = 0;
                            select_flower_usage_by_type_and_variety(from, to);
                            break;
                        }
                        case 5: {
                            char from[11], to[11];
                            printf("Enter from date (YYYY-MM-DD): ");
                            fgets(from, sizeof(from), stdin);
                            from[strcspn(from, "\n")] = 0;
                            printf("Enter to date (YYYY-MM-DD): ");
                            fgets(to, sizeof(to), stdin);
                            to[strcspn(to, "\n")] = 0;
                            select_composition_sales(from, to);
                            break;
                        }
                        case 0: break;
                        default: printf("Invalid choice.\n");
                    }
                } while (report_choice != 0);
                break;
            }
            case 2:
                flower_management_menu();
                break;
            case 3:
                composition_management_menu();
                break;
            case 4: {
                price_policy_menu();
                break;
            }
            case 0:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice.\n");
        }
    } while (choice != 0);
}
