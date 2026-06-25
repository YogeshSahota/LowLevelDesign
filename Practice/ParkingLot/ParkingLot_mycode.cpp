#include<iostream>
#include<chrono>
#include<unordered_map>
#include<queue>

using namespace std;

enum class VehicleType{
    BIKE,
    CAR,
    TRUCK
};

enum class SpotType{
    MOTORCYCLE,
    COMPACT,
    LARGE
};

class Vehicle{
    private:
        string licenseNumber;
        VehicleType type;
    public:
        Vehicle(string licenseNumber, VehicleType type){
            this->licenseNumber=licenseNumber;
            this->type=type;
        }

        string getLicenseNumber(){
            return this->licenseNumber;
        }

        VehicleType getVehicleType(){
            return this->type;
        }
};

class Bike: public Vehicle{
    public:
        Bike(string licenseNumber):Vehicle(licenseNumber,VehicleType::BIKE) {}
};
class Car: public Vehicle{
    public:
        Car(string licenseNumber):Vehicle(licenseNumber,VehicleType::CAR) {}
};
class Truck: public Vehicle{
    public:
        Truck(string licenseNumber):Vehicle(licenseNumber,VehicleType::TRUCK) {}
};

class VehicleFactory{
    public:
        static Vehicle* createVehice(string licenseNumber, VehicleType type){
            switch (type)
            {
            case VehicleType::BIKE :
                return new Bike(licenseNumber);
                break;
            case VehicleType::CAR :
                return new Car(licenseNumber);
                break;
            case VehicleType::TRUCK :
                return new Truck(licenseNumber);
            break;
            default:
                break;
            }
            return new Car(licenseNumber);
        }
};

class ParkingSpot{
    private:
        int spotId;
        SpotType type;
        bool occupied;
        Vehicle* vehicle;
    public:
        ParkingSpot(int id, SpotType type){
            this->spotId = id;
            this->type = type;
            this->occupied = false;
            this->vehicle = nullptr;
        }

        virtual bool canFitVehicle(Vehicle* v) = 0;

        void parkVehicle(Vehicle* v){
            this->occupied = true;
            this->vehicle = v;
        }

        void removeVehicle(){
            this->occupied = false;
            this->vehicle = nullptr;
        }

        Vehicle* getVehicle(){
            return this->vehicle;
        }

        bool isOccupied(){
            return occupied;
        }

        int getSpotId() const
        {
            return spotId;
        }
};

class MotorcycleSpot:public ParkingSpot{
    public:
        MotorcycleSpot(int spotId):ParkingSpot(spotId,SpotType::MOTORCYCLE){}

        bool canFitVehicle(Vehicle* v) override {
            return v->getVehicleType() == VehicleType::BIKE;
        }
};

class CompactSpot:public ParkingSpot{
    public:
        CompactSpot(int spotId):ParkingSpot(spotId,SpotType::COMPACT){}

        bool canFitVehicle(Vehicle* v) override {
            return v->getVehicleType() == VehicleType::CAR;
        }
};

class LargeSpot:public ParkingSpot{
    public:
        LargeSpot(int spotId):ParkingSpot(spotId,SpotType::LARGE){}

        bool canFitVehicle(Vehicle* v) override {
            return v->getVehicleType() == VehicleType::TRUCK;
        }
};

class Ticket{
    private:
        string ticketId;
        ParkingSpot* spot;
        Vehicle* vehicle;
        chrono::system_clock::time_point entryTime;
    public:
        Ticket(string t,ParkingSpot* s, Vehicle* v){
            this->ticketId = t;
            this->spot = s;
            this->vehicle = v;
            this->entryTime = chrono::system_clock::now();
        }

        string getTicketId(){
            return this->ticketId;
        }
        
        ParkingSpot* getSpot(){
            return this->spot;
        }
        chrono::system_clock::time_point getEntryTime() const {
            return entryTime;
        }
};

class PricingStrategy{
    public:
        virtual double calculateFee(Ticket* ticket) = 0;
};

class HourlyPricingStrategy:public PricingStrategy{
    private:
        double rate;
    public:
        HourlyPricingStrategy(double rate){
            this->rate = rate;
        }
        double calculateFee(Ticket* ticket) override {
            chrono::system_clock::time_point exitTime = chrono::system_clock::now();
            auto duration = chrono::duration_cast<chrono::hours>(exitTime - ticket->getEntryTime());
            long long hours = max(1LL,duration.count());
            return rate*hours;
        };
};

class ParkingFloor{
    private:
        int floorNo;
        vector<ParkingSpot*> spots;
        unordered_map<VehicleType,queue<ParkingSpot*>> availableSpots;
    public:
        ParkingFloor(int floorNo){
            this->floorNo = floorNo;
        }

        void addSpot(VehicleType type, ParkingSpot* spot){
            spots.push_back(spot);
            availableSpots[type].push(spot);
        }

        ParkingSpot* getAvailableSpot(VehicleType type){
            auto &spotQueue = availableSpots[type];
            if(spotQueue.empty()) return nullptr;
            auto spot = spotQueue.front();
            spotQueue.pop();
            return spot;
        }

        void releaseSpot(VehicleType type, ParkingSpot* spot){
            availableSpots[type].push(spot);
        }
};

class ParkingLot{
    private:
        int ticketCount;
        vector<ParkingFloor*> floors;
        unordered_map<string,Ticket*> activeTickets;
        PricingStrategy* pricingStrategy;
    public:
        ParkingLot(PricingStrategy* ps){
            this->pricingStrategy=ps;
            this->ticketCount=0;
        }
        void addFloor(ParkingFloor* floor){
            floors.push_back(floor);
        }
        Ticket* parkVehicle(Vehicle* vehicle){
            for(auto floor:floors){
                auto spot = floor->getAvailableSpot(vehicle->getVehicleType());
                if(spot){
                    spot->parkVehicle(vehicle);
                    string ticketId = "T" + to_string(++ticketCount);
                    Ticket* ticket = new Ticket(ticketId,spot,vehicle);
                    activeTickets[ticketId] = ticket;
                    return ticket;
                }
            }
            throw runtime_error("Parking Full");
        }
        double unParkVehicle(string ticketId){
            auto it = activeTickets.find(ticketId);

            if(it==activeTickets.end()){
                throw runtime_error("Invalid Ticket");
            }

            auto ticket = it->second;
            double fee = pricingStrategy->calculateFee(ticket);
            ticket->getSpot()->removeVehicle();
            activeTickets.erase(ticketId);

            return fee;
        }
};

int main()
{

    auto pricing = new HourlyPricingStrategy(20.0);

    ParkingLot parkingLot(move(pricing));

    auto floor1 = new ParkingFloor(1);

    floor1->addSpot(VehicleType::BIKE,new MotorcycleSpot(1));
    floor1->addSpot(VehicleType::CAR,new CompactSpot(2));
    floor1->addSpot(VehicleType::TRUCK,new LargeSpot(3));

    parkingLot.addFloor(floor1);

    auto car =
        VehicleFactory::createVehice("TN-01-1234",VehicleType::CAR);

    auto ticket =
        parkingLot.parkVehicle(car);

    cout << "Vehicle parked. Ticket ID: "
         << ticket->getTicketId()
         << endl;

    double fee =
        parkingLot.unParkVehicle(
            ticket->getTicketId());

    cout << "Parking Fee: "
         << fee
         << endl;

    return 0;
}
