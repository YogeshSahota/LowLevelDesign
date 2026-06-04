#include <iostream>
#include <string>
#include <math.h>

using namespace std;

struct PaymentRequest{
    string sender;
    string reciever;
    double amount;
    string currency;

    PaymentRequest(const string s, const string r, const double amt, const string c){
        sender = s;
        reciever = r;
        amount = amt;
        currency = c;
    }
};

class BankingSystem{
    public:
        virtual bool processPayment(int amt) = 0;
};

class RazorpayBankingSystem: public BankingSystem{
    public:
        bool processPayment(int amt) override{
            cout << "[Razorpay] processing payment of " << amt << endl;
            int ans = rand()%100;
            return ans<90;
        };
};

class PaytmBankingSystem: public BankingSystem{
    public:
        bool processPayment(int amt) override{
            cout << "[Paytm] processing payment of " << amt << endl;
            int ans = rand()%100;
            return ans<80;
        };
};

class PaymentGateway{
    protected:
        BankingSystem* bs;
    public:
        PaymentGateway(){
            bs = nullptr;
        }
        virtual bool processPayment(PaymentRequest* pr){
            if(!validateRequest(pr)){
                cout << "Validation falied for " << pr->sender <<endl;
                return false;
            }
            if(!initiatePayment(pr)){
                cout << "Payment processing falied for " << pr->sender <<endl;
                return false;
            }
            if(!confirmPayment(pr)){
                cout << "Payment confirmation falied for " << pr->sender <<endl;
                return false;
            }
            return true;
        }

        
        virtual bool validateRequest(PaymentRequest* pr) = 0;
        virtual bool initiatePayment(PaymentRequest* pr) = 0;
        virtual bool confirmPayment(PaymentRequest* pr) = 0;


};

class PaytmPaymentGateway: public PaymentGateway{
    public:
        PaytmPaymentGateway(){
            bs = new PaytmBankingSystem();
        }

        bool validateRequest(PaymentRequest* pr) override {
            cout << "[Patym] validating the request from "<< pr->sender <<endl;
            if(pr->amount<0||pr->currency!="INR") return false;
            return true;
        }
        bool initiatePayment(PaymentRequest* pr) override {
            cout << "[Paytm] Initiating the payment from " << pr->sender << endl;
            return bs->processPayment(pr->amount);
        }
        bool confirmPayment(PaymentRequest* pr) override {
            cout << "[Paytm] Confirming the payment from " << pr->sender << endl;
            return true;
        }
};

class RazorpayPaymentGateway: public PaymentGateway{
    public:
        RazorpayPaymentGateway(){
            bs = new RazorpayBankingSystem();
        }

        bool validateRequest(PaymentRequest* pr) override {
            cout << "[Razorpay] validating the request from "<< pr->sender <<endl;
            if(pr->amount<0) return false;
            return true;
        }
        bool initiatePayment(PaymentRequest* pr) override {
            cout << "[Razorpay] Initiating the payment from " << pr->sender << endl;
            return bs->processPayment(pr->amount);
        }
        bool confirmPayment(PaymentRequest* pr) override {
            cout << "[Razorpay] Confirming the payment from " << pr->sender << endl;
            return true;
        }
};

class PaymentGatewayProxy: public PaymentGateway{
    private:
        PaymentGateway* realPG;
        int maxRetries;
    public:
        PaymentGatewayProxy(PaymentGateway* pg, int mx){
            realPG = pg;
            maxRetries = mx;
        }

        bool processPayment(PaymentRequest* pr) override {
            for (int attempt = 0; attempt < maxRetries; ++attempt) {
                if (attempt > 0) {
                    cout << "[Proxy] Retrying payment (attempt " << (attempt+1)
                            << ") for " << pr->sender << ".\n";
                }
                int result = realPG->processPayment(pr);
                if(result) return true;
            }
            cout << "[Proxy] Payment failed after " << (maxRetries) << " attempts for " << pr->sender << ".\n";
            return false;
        }
    protected:
        bool validateRequest(PaymentRequest* pr) override {
            return realPG->validateRequest(pr);
        }
        bool initiatePayment(PaymentRequest* pr) override {
            return realPG->initiatePayment(pr);
        }
        bool confirmPayment(PaymentRequest* pr) override {
            return realPG->confirmPayment(pr);
        }
};

enum class GatewayType{
    PAYTM,
    RAZORPAY
};

class GatewayFactory{
    private:
        static GatewayFactory* instance;
        // GatewayFactory(){
        //     instance = new GatewayFactory();
        // }
    public:
        static GatewayFactory* getInstance(){
            if(!instance){
                instance = new GatewayFactory();
            }
            return instance;
        }

        PaymentGateway* getGateway(GatewayType type){
            if(type == GatewayType::PAYTM){
                return new PaymentGatewayProxy(new PaytmPaymentGateway(),3);
            }
            // if(type == GatewayType::RAZORPAY){
                return new PaymentGatewayProxy(new RazorpayPaymentGateway(),1);
            // }
        }
};
GatewayFactory* GatewayFactory::instance = nullptr;

class PaymentService{
    private:
        static PaymentService* instance;
        PaymentGateway* gateway;

        PaymentService(){
            gateway = nullptr;
        }
    public:
        static PaymentService* getInstance(){
            if(!instance){
                instance = new PaymentService();
            }
            return instance;
        }

        void setGateway(PaymentGateway* g){
            if (gateway) delete gateway;
            gateway = g;
        }
        bool processPayment(PaymentRequest* pr){
            if(!gateway){
                cout << "Payment gateway is not set" << endl;
                return false;
            }
            return gateway->processPayment(pr);
        }
};
PaymentService* PaymentService::instance = nullptr;

class PaymentController {
private:
    static PaymentController instance;
    PaymentController() {}
    PaymentController(const PaymentController&) = delete;
    PaymentController& operator=(const PaymentController&) = delete;
public:
    static PaymentController& getInstance() {
        return instance;
    }
    bool handlePayment(GatewayType type, PaymentRequest* req) {
        PaymentGateway* paymentGateway = GatewayFactory::getInstance()->getGateway(type);
        PaymentService::getInstance()->setGateway(paymentGateway);
        return PaymentService::getInstance()->processPayment(req);
    }
};

PaymentController PaymentController::instance;

// ----------------------------
// Main: Client code now goes through controller
// ----------------------------
int main() {

    // srand(static_cast<unsigned>(time(nullptr)));

    PaymentRequest* req1 = new PaymentRequest("Aditya", "Shubham", 1000.0, "INR");

    cout << "Processing via Paytm\n";
    cout << "------------------------------\n";
    bool res1 = PaymentController::getInstance().handlePayment(GatewayType::PAYTM, req1);
    cout << "Result: " << (res1 ? "SUCCESS" : "FAIL") << "\n";
    cout << "------------------------------\n\n";

    PaymentRequest* req2 = new PaymentRequest("Shubham", "Aditya", 500.0, "USD");

    cout << "Processing via Razorpay\n";
    cout << "------------------------------\n";
    bool res2 = PaymentController::getInstance().handlePayment(GatewayType::RAZORPAY, req2);
    cout << "Result: " << (res2 ? "SUCCESS" : "FAIL") << "\n";
    cout << "------------------------------\n";

    return 0;
}