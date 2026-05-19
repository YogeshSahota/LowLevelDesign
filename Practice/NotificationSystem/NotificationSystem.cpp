#include <bits/stdc++.h>

using namespace std;

class INotification {
    public:
        virtual string getContent() = 0;
};

class SimpleNotification: public INotification {
    private:
        string text;

    public:
        SimpleNotification(string text) {
            this->text = text;
        }

        string getContent() override {
            return text;
        }
};

class INotificationDecorator: public INotification {
    protected:
        INotification* notification;
    
    public:
        INotificationDecorator(INotification* notification) {
            this->notification=notification;
        }
};

class TimestampDecorator: public INotificationDecorator {
    public:
        TimestampDecorator(INotification* n):INotificationDecorator(n){ }

        string getContent() {
            return "[2025-04-13 14:22:00] " + notification->getContent();
        }
};

class SignatureDecorator: public INotificationDecorator {
    private:
        string signature;

    public:
        SignatureDecorator(INotification* n, string sig):INotificationDecorator(n){ 
            string signature = sig;
         }

        string getContent() {
            return notification->getContent() + "\n-- " + signature + "\n\n";
        }
};

class IObserver {
    public:
        virtual void update() = 0;
};

class IObservable {
    public:
        virtual void addObserver(IObserver* o) = 0;
        virtual void removeObserver(IObserver* o) = 0;
        virtual void notifyObservers() = 0;
};

class NotificationObservable: public IObservable {
    private:
        vector<IObserver*> observers;
        INotification* currentNotification;

    public:
        NotificationObservable(){
            currentNotification=nullptr;
        }
        void addObserver(IObserver* o) override {
            observers.push_back(o);
        }

        void removeObserver(IObserver* o) override {
            observers.erase(remove(observers.begin(), observers.end(), o), observers.end());
        }

        void notifyObservers() {
            for(auto observer:observers){
                observer->update();
            }
        }

        void setNotification(INotification* notification) {
            if (currentNotification != nullptr) {
                delete currentNotification;
            }
            currentNotification = notification;
            notifyObservers();
        }

        INotification* getNotification() {
            return currentNotification;
        }

        string getNotificationContent() {
            return currentNotification->getContent();
        }

        ~NotificationObservable() {
            if (currentNotification != NULL) {
                delete currentNotification;
            }
        }
};

class NotificationService{
    private:
        static NotificationService* instance;
        NotificationObservable* observable;
        vector<INotification*> notifications;
        NotificationService(){ 
            observable = new NotificationObservable();
        }
    
    public:
        static NotificationService* getInstance() {
            if(!instance){
                instance = new NotificationService();
            }
            return instance;
        }

        void sendNotification(INotification* notification){
            notifications.push_back(notification);
            observable->setNotification(notification);
        }

        NotificationObservable* getObservable(){
            return observable;
        }

        ~NotificationService(){
            delete observable;
        }

};

NotificationService* NotificationService::instance = nullptr;

class Logger: public IObserver {
    private:
        NotificationObservable* notificationObservable;

    public:
        Logger(){
            this->notificationObservable = NotificationService::getInstance()->getObservable();
            notificationObservable->addObserver(this);
        }
        Logger(NotificationObservable* observable){
            this->notificationObservable = observable;
            notificationObservable->addObserver(this);
        }
        void update() override {
            cout << "Logging New Notification : \n" << notificationObservable->getNotificationContent();
        }
};

class INotificationStrategy{
    public:
        virtual void sendNotification(string content) = 0;
};

class EmailStrategy : public INotificationStrategy {
private:
    string emailId;
public:

    EmailStrategy(string emailId) {
        this->emailId = emailId;
    }

    void sendNotification(string content) override {
        // Simulate the process of sending an email notification, 
        // representing the dispatch of messages to users via email.​
        cout << "Sending email Notification to: " << emailId << "\n" << content;
    }
};

class SMSStrategy : public INotificationStrategy {
private:
    string mobileNumber;
public:

    SMSStrategy(string mobileNumber) {
        this->mobileNumber = mobileNumber;
    }

    void sendNotification(string content) override {
        // Simulate the process of sending an SMS notification, 
        // representing the dispatch of messages to users via SMS.​
        cout << "Sending SMS Notification to: " << mobileNumber << "\n" << content;
    }
};

class PopUpStrategy : public INotificationStrategy {
public:
    void sendNotification(string content) override {
        // Simulate the process of sending popup notification.
        cout << "Sending Popup Notification: \n" << content;
    }
};

class NotificationEngine: public IObserver {
    private:
        NotificationObservable* notificationObservable;
        vector<INotificationStrategy*> notificationStrategies;
    
    public:
        NotificationEngine(){
            this->notificationObservable=NotificationService::getInstance()->getObservable();
            this->notificationObservable->addObserver(this);
        }
        NotificationEngine(NotificationObservable* notificationObservable){
            this->notificationObservable=notificationObservable;
            this->notificationObservable->addObserver(this);
        }
        void addNotificationStrategy(INotificationStrategy* ns){
            notificationStrategies.push_back(ns);
        }
        void update() override {
            string content = notificationObservable->getNotificationContent();
            for(auto ns:notificationStrategies){
                ns->sendNotification(content);
            }
        }
};

int main() {
    // Create NotificationService.
    NotificationService* notificationService = NotificationService::getInstance();
   
    // Create Logger Observer
    Logger* logger = new Logger();

    // Create NotificationEngine observers.
    NotificationEngine* notificationEngine = new NotificationEngine();

    notificationEngine->addNotificationStrategy(new EmailStrategy("random.person@gmail.com"));
    notificationEngine->addNotificationStrategy(new EmailStrategy("yogeshsahota@gmail.com"));
    notificationEngine->addNotificationStrategy(new SMSStrategy("+91 9876543210"));
    notificationEngine->addNotificationStrategy(new PopUpStrategy());

    // Create a notification with decorators.
    INotification* notification = new SimpleNotification("Your order has been shipped!");
    notification = new TimestampDecorator(notification);
    notification = new SignatureDecorator(notification, "Customer Care");
    
    notificationService->sendNotification(notification);

    delete logger;
    delete notificationEngine;
    return 0;
}