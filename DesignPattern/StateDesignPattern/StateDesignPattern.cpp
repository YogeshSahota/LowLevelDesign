#include <iostream>

using namespace std;

class VendingMachine;

class NoCoinState;
class HasCoinState;
class DispenseState;
class SoldOutState;

class VendingState{
    public:
        virtual VendingState* insertCoin(VendingMachine* machine, int coin) = 0;
        virtual VendingState* selectIem(VendingMachine* machine) = 0;
        virtual VendingState* dispense(VendingMachine* machine) = 0;
        virtual VendingState* returnCoin(VendingMachine* machine) = 0;
        virtual VendingState* refill(VendingMachine* machine, int qty) = 0;

        virtual string getStateName() = 0;
};

class VendingMachine{
    private:
        VendingState* currentState;
        int itemPrice;
        int itemCount;
        int insertedCoins;

        VendingState* noCoinState;
        VendingState* hasCoinState;
        VendingState* dispenseState;
        VendingState* soldOutState;

    public:

        VendingMachine(int price, int qty);

        void insertCoin(int coin);
        void selectItem();
        void dispense();
        void returnCoin();
        void refill(int qty);

        VendingState* getCurrentState(){
            return this->currentState;
        }
        VendingState* getNoCoinState(){
            return this->noCoinState;
        }
        VendingState* getHasCoinState(){
            return this->hasCoinState;
        }
        VendingState* getDispenseState(){
            return this->dispenseState;
        }
        VendingState* getSoldOutState(){
            return this->soldOutState;
        }

        void setInsertedCoins(int coins){
            this->insertedCoins = coins;
        }

        void decrementItemCount(){
            this->itemCount--;
        }

        void incrementItemCount(int count = 1){
            this->itemCount+=count;
        }

        int getItemCount(){
            return this->itemCount;
        }

        void addCoins(int coins){
            this->insertedCoins+=coins;
        }

        int getCoins(){
            return this->insertedCoins;
        }

        void setPrice(int price){
            this->itemPrice=price;
        }

        int getPrice(){
            return this->itemPrice;
        }

        void printStatus(){
            cout << "\n--- Vending Machine Status ---" << endl;
            cout << "Items remaining: " << itemCount << endl;
            cout << "Inserted coin: Rs " << insertedCoins << endl;
            cout << "Current state: " << currentState->getStateName() << endl << endl;
        };

};

class NoCoinState: public VendingState{
    public:
        VendingState* insertCoin(VendingMachine* machine, int coin) override {
            cout << "Coin inserted. Current balance: Rs " << coin <<endl;
            machine->setInsertedCoins(coin);
            return machine->getHasCoinState();
        };
        VendingState* selectIem(VendingMachine* machine) override {
            cout << "No coin inserted. Please insert coin" << endl;
            return machine->getCurrentState();
        };
        VendingState* dispense(VendingMachine* machine) override {
            cout << "No coin inserted. Please insert coin" << endl;
            return machine->getCurrentState();
        };
        VendingState* returnCoin(VendingMachine* machine) override {
            cout << "No coin inserted. Please insert coin" << endl;
            return machine->getCurrentState();
        };
        VendingState* refill(VendingMachine* machine, int qty) override {
            cout << "Refilling Items" << endl;
            cout << "Added " << qty << " items to the machine." <<endl;
            machine->incrementItemCount(qty);
            return machine->getCurrentState();
        };

        string getStateName() override {
            return "NO_COIN";
        }
};

class HasCoinState: public VendingState{
    public:
        VendingState* insertCoin(VendingMachine* machine, int coin) override {
            machine->addCoins(coin);
            cout << "Additional coin inserted. Current balance: Rs " << machine->getCoins() <<endl;
            return machine->getCurrentState();
        };
        VendingState* selectIem(VendingMachine* machine) override {
            if(machine->getCoins()>=machine->getPrice()){
                cout << "Selected the item" << endl;
                int balance = machine->getCoins() - machine->getPrice();
                machine->setInsertedCoins(0);
                cout << "Returning Balance: Rs "<<balance<<endl;
                return machine->getDispenseState();
            }

            int neededCoins = machine->getPrice() - machine->getCoins();
            cout << "Insufficient funds. Need Rs " << neededCoins << " more." <<endl;
            return machine->getCurrentState();
        };
        VendingState* dispense(VendingMachine* machine) override {
            cout << "First need to select the Item" << endl;
            return machine->getCurrentState();
        };
        VendingState* returnCoin(VendingMachine* machine) override {
            cout << "Coin returned: Rs " << machine->getCoins() <<endl;
            machine->setInsertedCoins(0);
            return machine->getNoCoinState();
        };
        VendingState* refill(VendingMachine* machine, int qty) override {
            cout << "Can't refill at this state." <<endl;
            return machine->getCurrentState();
        };

        string getStateName() override {
            return "HAS_COIN";
        }
};

class DispenseState: public VendingState{
    public:
        VendingState* insertCoin(VendingMachine* machine, int coin) override {
            cout << "Please wait, already dispensing item. Coin returned: Rs " << coin <<endl;
            return machine->getCurrentState();
        };
        VendingState* selectIem(VendingMachine* machine) override {
            cout << "Item Already selected!" << endl;
            return machine->getCurrentState();
        };
        VendingState* dispense(VendingMachine* machine) override {
            cout << "Item Dispensed!" << endl;
            machine->decrementItemCount();
            if(machine->getItemCount()){
                return machine->getNoCoinState();
            }
            cout << "All items sold out!" <<endl;
            return machine->getSoldOutState();
        };
        VendingState* returnCoin(VendingMachine* machine) override {
            cout << "Despensing the item, can't return the coins" << endl;
            return machine->getCurrentState();
        };
        VendingState* refill(VendingMachine* machine, int qty) override {
            cout << "Can't refill at this state." <<endl;
            return machine->getCurrentState();
        };

        string getStateName() override {
            return "DESPENSE";
        }
};

class SoldOutState: public VendingState{
    public:
        VendingState* insertCoin(VendingMachine* machine, int coin) override {
            cout << "All items sold out! Coin returned: Rs " << coin <<endl;
            return machine->getCurrentState();
        };
        VendingState* selectIem(VendingMachine* machine) override {
            cout << "All items sold out!" << endl;
            return machine->getCurrentState();
        };
        VendingState* dispense(VendingMachine* machine) override {
            cout << "All items sold out!n" << endl;
            return machine->getCurrentState();
        };
        VendingState* returnCoin(VendingMachine* machine) override {
            cout << "All items sold out!" << endl;
            return machine->getCurrentState();
        };
        VendingState* refill(VendingMachine* machine, int qty) override {
            cout << "Refilling Items" << endl;
            cout << "Added " << qty << " items to the machine." <<endl;
            machine->incrementItemCount(qty);
            return machine->getNoCoinState();
        };

        string getStateName() override {
            return "SOLD_OUT";
        }
};

VendingMachine::VendingMachine(int price, int qty){
    this->itemPrice = price;
    this->itemCount = qty;
    this->insertedCoins=0;

    this->noCoinState = new NoCoinState();
    this->hasCoinState = new HasCoinState();
    this->dispenseState = new DispenseState();
    this->soldOutState = new SoldOutState();

    if(qty==0) this->currentState = this->soldOutState;
    else this->currentState = this->noCoinState;
}

void VendingMachine::insertCoin(int coin){
    currentState=currentState->insertCoin(this,coin);
};
void VendingMachine::selectItem(){
    currentState=currentState->selectIem(this);
};
void VendingMachine::dispense(){
    currentState=currentState->dispense(this);
};
void VendingMachine::returnCoin(){
    currentState=currentState->returnCoin(this);
};
void VendingMachine::refill(int qty){
    currentState=currentState->refill(this,qty);
};

int main() {
    cout << "=== Water Bottle VENDING MACHINE ===" <<endl;
    
    int itemCount = 2;
    int itemPrice = 20;

    VendingMachine machine(itemPrice,itemCount);
    machine.printStatus();
    
    // Test scenarios - each operation potentially changes state
    cout << "1. Trying to select item without coin:" <<endl;
    machine.selectItem();  // Should ask for coin, no state change
    machine.printStatus();
    
    cout << "2. Inserting coin:" <<endl;
    machine.insertCoin(10);  // State changes to HAS_COIN
    machine.printStatus();
    
    cout << "3. Selecting item with insufficient funds:" <<endl;
    machine.selectItem();  // Insufficient funds, stays in HAS_COIN
    machine.printStatus();
    
    cout << "4. Adding more coins:" <<endl;
    machine.insertCoin(10);  // Add more money, stays in HAS_COIN
    machine.printStatus();
    
    cout << "5. Selecting item Now" <<endl;
    machine.selectItem();  // State changes to SOLD
    machine.printStatus();
    
    cout << "6. Dispensing item:" <<endl;
    machine.dispense(); // State changes to NO_COIN (items remaining)
    machine.printStatus();
    
    cout << "7. Buying last item:" <<endl;
    machine.insertCoin(20);  // State changes to HAS_COIN
    machine.selectItem();  // State changes to SOLD
    machine.dispense(); // State changes to SOLD_OUT (no items left)
    machine.printStatus();
    
    cout << "8. Trying to use sold out machine:" <<endl;
    machine.insertCoin(5);  // Coin returned, stays in SOLD_OUT

    cout << "9. Trying to use sold out machine:" <<endl;
    machine.refill(2);
    machine.printStatus(); // State changes NO_COIN
    
    return 0;
}