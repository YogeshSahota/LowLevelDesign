
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <memory>
#include <string>
#include <chrono>
#include <stdexcept>

using namespace std;

enum class VehicleType
{
    BIKE,
    CAR,
    TRUCK
};

enum class SpotType
{
    MOTORCYCLE,
    COMPACT,
    LARGE
};

class Vehicle
{
protected:
    string licenseNumber;
    VehicleType vehicleType;

public:
    Vehicle(const string &licenseNumber,
            VehicleType vehicleType)
        : licenseNumber(licenseNumber),
          vehicleType(vehicleType) {}

    virtual ~Vehicle() = default;

    VehicleType getType() const
    {
        return vehicleType;
    }

    string getLicenseNumber() const
    {
        return licenseNumber;
    }
};

class Bike : public Vehicle
{
public:
    Bike(const string &number)
        : Vehicle(number, VehicleType::BIKE) {}
};

class Car : public Vehicle
{
public:
    Car(const string &number)
        : Vehicle(number, VehicleType::CAR) {}
};

class Truck : public Vehicle
{
public:
    Truck(const string &number)
        : Vehicle(number, VehicleType::TRUCK) {}
};

class VehicleFactory
{
public:
    static shared_ptr<Vehicle> createVehicle(
        VehicleType type,
        const string &license)
    {

        switch (type)
        {
        case VehicleType::BIKE:
            return make_shared<Bike>(license);

        case VehicleType::CAR:
            return make_shared<Car>(license);

        case VehicleType::TRUCK:
            return make_shared<Truck>(license);
        }

        return nullptr;
    }
};

class ParkingSpot
{
protected:
    int spotId;
    SpotType spotType;
    bool occupied;
    shared_ptr<Vehicle> vehicle;

public:
    ParkingSpot(int id, SpotType type)
        : spotId(id),
          spotType(type),
          occupied(false) {}

    virtual ~ParkingSpot() = default;

    virtual bool canFitVehicle(
        shared_ptr<Vehicle> vehicle) = 0;

    void parkVehicle(shared_ptr<Vehicle> vehicle)
    {
        this->vehicle = vehicle;
        occupied = true;
    }

    void removeVehicle()
    {
        vehicle = nullptr;
        occupied = false;
    }

    bool isOccupied() const
    {
        return occupied;
    }

    int getSpotId() const
    {
        return spotId;
    }
};

class MotorcycleSpot : public ParkingSpot
{
public:
    MotorcycleSpot(int id)
        : ParkingSpot(id, SpotType::MOTORCYCLE) {}

    bool canFitVehicle(
        shared_ptr<Vehicle> vehicle) override
    {

        return vehicle->getType() == VehicleType::BIKE;
    }
};

class CompactSpot : public ParkingSpot
{
public:
    CompactSpot(int id)
        : ParkingSpot(id, SpotType::COMPACT) {}

    bool canFitVehicle(
        shared_ptr<Vehicle> vehicle) override
    {

        return vehicle->getType() == VehicleType::CAR;
    }
};

class LargeSpot : public ParkingSpot
{
public:
    LargeSpot(int id)
        : ParkingSpot(id, SpotType::LARGE) {}

    bool canFitVehicle(
        shared_ptr<Vehicle> vehicle) override
    {

        return vehicle->getType() == VehicleType::TRUCK;
    }
};

class Ticket
{
private:
    string ticketId;
    shared_ptr<Vehicle> vehicle;
    shared_ptr<ParkingSpot> spot;
    chrono::system_clock::time_point entryTime;

public:
    Ticket(
        const string &ticketId,
        shared_ptr<Vehicle> vehicle,
        shared_ptr<ParkingSpot> spot)
        : ticketId(ticketId),
          vehicle(vehicle),
          spot(spot),
          entryTime(chrono::system_clock::now()) {}

    string getTicketId() const
    {
        return ticketId;
    }

    shared_ptr<ParkingSpot> getSpot() const
    {
        return spot;
    }

    chrono::system_clock::time_point getEntryTime() const
    {
        return entryTime;
    }
};

class PricingStrategy
{
public:
    virtual double calculateFee(
        const Ticket &ticket) = 0;

    virtual ~PricingStrategy() = default;
};

class HourlyPricingStrategy : public PricingStrategy
{
private:
    double hourlyRate;

public:
    HourlyPricingStrategy(double rate = 20.0)
        : hourlyRate(rate) {}

    double calculateFee(
        const Ticket &ticket) override
    {

        auto exitTime = chrono::system_clock::now();

        auto duration =
            chrono::duration_cast<chrono::hours>(
                exitTime - ticket.getEntryTime());

        long long hours =
            max(1LL, duration.count());

        return hours * hourlyRate;
    }
};

class ParkingFloor
{
private:
    int floorNumber;

    vector<shared_ptr<ParkingSpot>> spots;

    unordered_map<
        VehicleType,
        queue<shared_ptr<ParkingSpot>>>
        availableSpots;

public:
    ParkingFloor(int floorNo)
        : floorNumber(floorNo) {}

    void addSpot(
        shared_ptr<ParkingSpot> spot,
        VehicleType vehicleType)
    {

        spots.push_back(spot);
        availableSpots[vehicleType].push(spot);
    }

    shared_ptr<ParkingSpot> getAvailableSpot(
        VehicleType type)
    {

        auto &q = availableSpots[type];

        if (q.empty())
            return nullptr;

        auto spot = q.front();
        q.pop();

        return spot;
    }

    void releaseSpot(
        shared_ptr<ParkingSpot> spot,
        VehicleType type)
    {

        availableSpots[type].push(spot);
    }
};

class ParkingLot
{
private:
    vector<shared_ptr<ParkingFloor>> floors;

    unordered_map<string, shared_ptr<Ticket>>
        activeTickets;

    unique_ptr<PricingStrategy> pricingStrategy;

    int ticketCounter = 1;

public:
    ParkingLot(
        unique_ptr<PricingStrategy> strategy)
        : pricingStrategy(move(strategy)) {}

    void addFloor(
        shared_ptr<ParkingFloor> floor)
    {

        floors.push_back(floor);
    }

    shared_ptr<Ticket> parkVehicle(
        shared_ptr<Vehicle> vehicle)
    {

        for (auto &floor : floors)
        {

            auto spot =
                floor->getAvailableSpot(
                    vehicle->getType());

            if (spot)
            {

                spot->parkVehicle(vehicle);

                string ticketId =
                    "T" + to_string(ticketCounter++);

                auto ticket =
                    make_shared<Ticket>(
                        ticketId,
                        vehicle,
                        spot);

                activeTickets[ticketId] = ticket;

                return ticket;
            }
        }

        throw runtime_error("Parking Full");
    }

    double unParkVehicle(
        const string &ticketId)
    {

        auto it = activeTickets.find(ticketId);

        if (it == activeTickets.end())
        {
            throw runtime_error("Invalid Ticket");
        }

        auto ticket = it->second;

        ticket->getSpot()->removeVehicle();

        double fee =
            pricingStrategy->calculateFee(
                *ticket);

        activeTickets.erase(ticketId);

        return fee;
    }
};

int main()
{

    auto pricing =
        make_unique<HourlyPricingStrategy>();

    ParkingLot parkingLot(move(pricing));

    auto floor1 =
        make_shared<ParkingFloor>(1);

    floor1->addSpot(
        make_shared<MotorcycleSpot>(1),
        VehicleType::BIKE);

    floor1->addSpot(
        make_shared<CompactSpot>(2),
        VehicleType::CAR);

    floor1->addSpot(
        make_shared<LargeSpot>(3),
        VehicleType::TRUCK);

    parkingLot.addFloor(floor1);

    auto car =
        VehicleFactory::createVehicle(
            VehicleType::CAR,
            "TN-01-1234");

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
