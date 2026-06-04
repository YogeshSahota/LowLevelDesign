#include <iostream>

using namespace std;

class MoneyHandler {
    protected:
        MoneyHandler* nextHandler;
    
    public:
        MoneyHandler(){
            nextHandler = nullptr;
        }
        void setNextHandler(MoneyHandler* next){
            nextHandler = next;
        }
        virtual void dispense(int amt) = 0;
};

class ThousandHandler: public MoneyHandler{
    private:
        int numNotes;
    
    public:
        ThousandHandler(int n){
            numNotes=n;
        }
        void dispense(int amt) override {
            int notesNeeded = amt/1000;

            if(numNotes<notesNeeded){
                notesNeeded=numNotes;
                numNotes=0;
            }else{
                numNotes-=notesNeeded;
            }

            if(notesNeeded){
                cout << "Dispensing " << notesNeeded << " x 1000 notes" <<endl;
            }

            int remainingAmt = amt - (notesNeeded*1000);

            if(remainingAmt){
                if(nextHandler){
                    nextHandler->dispense(remainingAmt);
                }else{
                    cout << "ATM out of entered amount" << endl;
                }
            }
        }
};

class FiveHundredHandler: public MoneyHandler{
    private:
        int numNotes;
    
    public:
        FiveHundredHandler(int n){
            numNotes=n;
        }
        void dispense(int amt) override {
            int notesNeeded = amt/500;

            if(numNotes<notesNeeded){
                notesNeeded=numNotes;
                numNotes=0;
            }else{
                numNotes-=notesNeeded;
            }

            if(notesNeeded){
                cout << "Dispensing " << notesNeeded << " x 500 notes" <<endl;
            }

            int remainingAmt = amt - (notesNeeded*500);

            if(remainingAmt){
                if(nextHandler){
                    nextHandler->dispense(remainingAmt);
                }else{
                    cout << "ATM out of entered amount" << endl;
                }
            }
        }
};

class TwoHundredHandler: public MoneyHandler{
    private:
        int numNotes;
    
    public:
        TwoHundredHandler(int n){
            numNotes=n;
        }
        void dispense(int amt) override {
            int notesNeeded = amt/200;

            if(numNotes<notesNeeded){
                notesNeeded=numNotes;
                numNotes=0;
            }else{
                numNotes-=notesNeeded;
            }

            if(notesNeeded){
                cout << "Dispensing " << notesNeeded << " x 200 notes" <<endl;
            }

            int remainingAmt = amt - (notesNeeded*200);

            if(remainingAmt){
                if(nextHandler){
                    nextHandler->dispense(remainingAmt);
                }else{
                    cout << "ATM out of entered amount" << endl;
                }
            }
        }
};

class HundredHandler: public MoneyHandler{
    private:
        int numNotes;
    
    public:
        HundredHandler(int n){
            numNotes=n;
        }
        void dispense(int amt) override {
            int notesNeeded = amt/100;

            if(numNotes<notesNeeded){
                notesNeeded=numNotes;
                numNotes=0;
            }else{
                numNotes-=notesNeeded;
            }

            if(notesNeeded){
                cout << "Dispensing " << notesNeeded << " x 100 notes" <<endl;
            }

            int remainingAmt = amt - (notesNeeded*100);

            if(remainingAmt){
                if(nextHandler){
                    nextHandler->dispense(remainingAmt);
                }else{
                    cout << "ATM out of entered amount" << endl;
                }
            }
        }
};

int main() {
    // Creating handlers for each note type
    MoneyHandler* thousandHandler = new ThousandHandler(3);
    MoneyHandler* fiveHundredHandler = new FiveHundredHandler(5);
    MoneyHandler* twoHundredHandler= new TwoHundredHandler(10);
    MoneyHandler* hundredHandler= new HundredHandler(20);

    // Setting up the chain of responsibility
    thousandHandler->setNextHandler(fiveHundredHandler);
    fiveHundredHandler->setNextHandler(twoHundredHandler);
    twoHundredHandler->setNextHandler(hundredHandler);

    int amountToWithdraw = 5300;

    // Initiating the chain
    cout << "\nDispensing amount: ₹" << amountToWithdraw << endl;
    thousandHandler->dispense(amountToWithdraw);

    return 0;
}