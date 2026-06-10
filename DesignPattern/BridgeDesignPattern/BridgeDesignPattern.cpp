#include <iostream>

using namespace std;

class Engine{
    public:
        virtual void start() = 0;
};

class PetrolEngine: public Engine{
    public:
        void start() override {
            cout << "Petrol Engine igniting" <<endl;
        }
};

class ElectricEngine: public Engine{
    public:
        void start() override {
            cout << "Electric Engine starting silently" <<endl;
        }
};

class DieselEngine: public Engine{
    public:
        void start() override {
            cout << "Diesel Engine starting up" <<endl;
        }
};

class Car{
    protected:
        Engine* engine;
    public:
        Car(Engine* e){
            engine = e;
        }
    virtual void drive() = 0;
};

class Sedan: public Car{
    public:
        Sedan(Engine* e):Car(e){

        }

        void drive() override {
            engine->start();
            cout << "Driving Sedan" <<endl;
        }

};

class SUV: public Car{
    public:
        SUV(Engine* e):Car(e){

        }

        void drive() override {
            engine->start();
            cout << "Driving SUV" <<endl;
        }

};

int main(){
    Engine* petrolEngine = new PetrolEngine();
    Engine* dieselEngine = new DieselEngine();
    Engine* electricEngine = new ElectricEngine();

    Car* myCar = new Sedan(petrolEngine);
    Car* yourCar = new SUV(dieselEngine);
    Car* CommonCar = new SUV(electricEngine);

    myCar->drive();
    yourCar->drive();
    CommonCar->drive();
}