/**
 * author: Team "Kvetachki"
 * @file report.h
 * @brief Analytics and reports for the Flower Greenhouse System
 */

#ifndef REPORT_H
#define REPORT_H

/**
 * @brief Calculate total revenue for a given period
 * @param from_date Start date (YYYY-MM-DD)
 * @param to_date End date (YYYY-MM-DD)
 * @return Total revenue, or -1 on error
 */
double select_revenue_for_period(const char *from_date, const char *to_date);

/**
 * @brief Find and display the most popular composition
 */
void select_most_popular_composition(void);

/**
 * @brief Display order statistics grouped by urgency (normal, 1day, 2days)
 */
void select_orders_by_urgency(void);

/**
 * @brief Count flower usage by type and variety within a period
 * @param from_date Start date (YYYY-MM-DD)
 * @param to_date End date (YYYY-MM-DD)
 */
void select_flower_usage_by_type_and_variety(const char *from_date, const char *to_date);

/**
 * @brief Display sold compositions and revenue within a period
 * @param from_date Start date (YYYY-MM-DD)
 * @param to_date End date (YYYY-MM-DD)
 */
void select_composition_sales(const char *from_date, const char *to_date);

#endif /* REPORT_H */
