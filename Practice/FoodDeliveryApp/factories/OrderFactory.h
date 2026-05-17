#ifndef ORDER_FACTOR_H
#define ORDER_FACTOR_H

#include "../models/Order.h"

class OrderFactory {
    public:
        virtual Order* createOrder(User* user, Cart* cart, Restaurant* restaurant, vector<MenuItem> &menuItems, PaymentStrategy* paymentStrategy, double totalCost,const string &orderType) = 0;

        virtual ~OrderFactory() {}
};

#endif