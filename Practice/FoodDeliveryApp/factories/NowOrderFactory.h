#ifndef NOW_ORDER_FACTORY
#define NOW_ORDER_FACTORY

#include "OrderFactory.h"
#include "../models/DeliveryOrder.h"
#include "../models/PickupOrder.h"
#include "../utils/TimeUtils.h"

using namespace std;

class NowOrderFactory: public OrderFactory {
    public:
        Order* createOrder(User* user, Cart* cart, Restaurant* restaurant, vector<MenuItem> &menuitems, PaymentStrategy* paymentStrategy, double totalCost, const string &orderType) override {
            Order* order = nullptr;

            if(orderType=="Delivery"){
                auto deliveryOrder = new DeliveryOrder();
                deliveryOrder->setUserAddress(user->getAddress());
                order=deliveryOrder;
            }else{
                auto pickupOrder = new PickupOrder();
                pickupOrder->setRestaurantAddress(restaurant->getLocation());
                order = pickupOrder;
            }
            order->setUser(user);
            order->setRestaurant(restaurant);
            order->setItems(menuitems);
            order->setPaymentStrategy(paymentStrategy);
            order->setScheduled(TimeUtils::getCurrentTime());
            order->setTotal(totalCost);

            return order;
        }
};

#endif