#ifndef SCHEDULED_ORDERfACTOR_H
#define SCHEDULED_ORDERfACTOR_H

#include "OrderFactory.h"
#include "../models/Order.h"
#include "../models/DeliveryOrder.h"
#include "../models/PickupOrder.h"

using namespace std;

class ScheduledOrderFactory: public OrderFactory {
    private:
        string schdeuledTime;

    public:
        ScheduledOrderFactory(const string &schdeuledTime){
            this->schdeuledTime = schdeuledTime;
        }
        Order* createOrder(User* user, Cart* cart, Restaurant* restaurant, vector<MenuItem> &menuitems, PaymentStrategy* paymentStrategy, double totalCost, const string &orderType) override{
            Order* order = nullptr;
            if(orderType=="Delivery"){
                auto deliveryOrder = new DeliveryOrder();
                deliveryOrder->setUserAddress(user->getAddress());
                order=deliveryOrder;
            }else{
                auto pickupOrder = new PickupOrder();
                pickupOrder->setRestaurantAddress(restaurant->getLocation());
                order=pickupOrder;
            }
            order->setUser(user);
            order->setRestaurant(restaurant);
            order->setItems(menuitems);
            order->setPaymentStrategy(paymentStrategy);
            order->setScheduled(schdeuledTime);
            order->setTotal(totalCost);
            return order;
        } 
};

#endif