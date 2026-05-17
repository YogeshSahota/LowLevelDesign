#ifndef UPI_PAYMENTSTRATEGY_H
#define UPI_PAYMENTSTRATEGY_H

#include "PaymentStrategy.h"
#include <iostream>
#include <string>

class UpiPaymentStrategy: public PaymentStrategy {
    private:
        string mobile;
    
    public:
        UpiPaymentStrategy(const string mobile){
            this->mobile=mobile;
        }

        void pay(double amount) override{
            cout << "Paid ₹" << amount << " using UPI (" << mobile << ")" << endl;
        }

};

#endif