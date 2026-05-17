#ifndef RESTAURANT_MANAGER
#define RESTAURANT_MANAGER

#include <string>
#include <vector>
#include <algorithm>
#include "../models/Restaurant.h"

using namespace std;

class RestaurantManager {
    private:
        vector<Restaurant*> restaurants;
        static RestaurantManager* instance;

        RestaurantManager(){
            // constructor
        }
    
    public:
        static RestaurantManager* getInstance() {
            if(!instance){
                instance = new RestaurantManager();
            }
            return instance;
        }

        void addRestaurant(Restaurant* restaurant){
            restaurants.push_back(restaurant);
        }

        vector<Restaurant*> searchByLocation(string loc){
            // cout << "searchByLocation ---> " << loc << endl;
            vector<Restaurant*> result;
            transform(loc.begin(), loc.end(), loc.begin(), ::tolower);
            for (auto r : restaurants) {
                string rl = r->getLocation();
                // cout<<rl<<endl;
                transform(rl.begin(), rl.end(), rl.begin(), ::tolower);
                if (rl == loc) {
                    result.push_back(r);
                }
            }
            return result;
        }
};

RestaurantManager* RestaurantManager::instance = nullptr;

#endif