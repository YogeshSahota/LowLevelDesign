#ifndef DELIVERY_ORDER_H
#define DELIVERY_ORDER_H

#include "Order.h"

class DeliveryOrder: public Order {
    private:
        string userAddress;
    
    public:
        DeliveryOrder() {
            userAddress = "";
        }

        string getType() const override {
            return "Delivery";
        }

        void setUserAddress(const string &addr){
            userAddress=addr;
        }

        string getUserAddress() const {
            return userAddress;
        }
};

#endif