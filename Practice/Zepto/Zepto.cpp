#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

class Product{
    private:
        int sku;
        string name;
        double price;

    public:
        Product(int id, string n, double p){
            sku=id;
            name=n;
            price=p;
        }

        int getSku(){
            return sku;
        }
        string getName(){
            return name;
        }
        double getPrice(){
            return price;
        }
};

class ProductFactory{
    public:
        static Product* createProduct(int sku){
            // in reality porduct will come from DB
            string name;
            double price;

            if (sku == 101) {
                name  = "Apple";
                price = 20;
            }
            else if (sku == 102) {
                name  = "Banana";
                price = 10;
            }
            else if (sku == 103) {
                name  = "Chocolate";
                price = 50;
            }
            else if (sku == 201) {
                name  = "T-Shirt";
                price = 500;
            }
            else if (sku == 202) {
                name  = "Jeans";
                price = 1000;
            }
            else {
                name  = "Item" + to_string(sku);
                price = 100;
            }
            return new Product(sku, name, price);
        }
};

class InventoryStore{
    public:
        
        virtual void addProduct(Product* p,int q) = 0;
        virtual void removeProduct(int sku, int q) = 0;
        virtual int checkQuantity(int sku) = 0;
        virtual vector<Product*> listAllProducts() = 0;
};

class DBInventoryStore: public InventoryStore{
    private:
        map<int,int>* stocks;
        map<int,Product*>* products;
    public:
        DBInventoryStore(){
            stocks = new map<int,int>();
            products = new map<int,Product*>();
        }
        ~DBInventoryStore(){
            for(auto it:*products){
                delete it.second;
            }
            delete stocks;
            delete products;
        }
        void addProduct(Product* p,int q) override{
            int sku = p->getSku();
            if(products->count(sku) == 0){
                (*products)[sku]=p;
            }else{
                delete p;
            }
            (*stocks)[sku]+=q;
        };
        void removeProduct(int sku, int q) override{
            if(stocks->count(sku)==0) return;

            int currentQuatity = (*stocks)[sku];
            int remainingQuantity = currentQuatity-q;

            if(remainingQuantity>0){
                (*stocks)[sku] = remainingQuantity;
            }else{
                (stocks)->erase(sku);
            }
        };
        int checkQuantity(int sku) override{
            if(stocks->count(sku)==0) return 0;
            return (*stocks)[sku];
        };
        vector<Product*> listAllProducts() override{
            vector<Product*> list;

            for(auto it:*stocks){
                int sku = it.first;
                int quantity = it.second;

                if(quantity>0&&(*products).count(sku)!=0){
                    list.push_back((*products)[sku]);
                }
            }
            return list;
        };
};

class InventoryManager{
    private:
        InventoryStore* store;
    public:
        InventoryManager(InventoryStore* s){
            store = s;
        }

        void addStock(int sku, int q){
            Product* pro = ProductFactory::createProduct(sku);
            store->addProduct(pro,q);
            cout << "[InventoryManager] Added SKU " << sku << " Qty " << q << endl;
        }

        void removeStock(int sku, int q){
            store->removeProduct(sku,q);
        }

        int getStock(int sku){
            return store->checkQuantity(sku);
        }
        vector<Product*> getAvailableProducts(){
            return store->listAllProducts();
        }
};

class ReplenishStrategy {
public:
    virtual void replenish(InventoryManager* manager, map<int,int> itemsToReplenish) = 0;
};

class WeeklyReplenish: public ReplenishStrategy{
    public:
        void replenish(InventoryManager* manager, map<int,int> itemsToreplenish){
            for(auto it:itemsToreplenish){
                int sku = it.first;
                int q = it.second;
                manager->addStock(sku,q);
            }
            cout << " Weekly Replenished ran" <<endl;
        }
};
class ThresholdReplenish: public ReplenishStrategy{
    private:
        int threshold;
    public:
        ThresholdReplenish(int threshold){
            this->threshold=threshold;
        }

        void replenish(InventoryManager* manager, map<int,int> itemsToreplenish){
            for(auto it:itemsToreplenish){
                int sku = it.first;
                int q = it.second;
                int currentQuantity = manager->getStock(sku);
                if(currentQuantity<threshold){
                    manager->addStock(sku,q);
                    cout << "Replenished [sku]:"<<sku<<" with " << q << " quantity" <<endl;
                }
            }
        }
};

class DarkStore{
    private:
        string name;
        double x,y;
        InventoryManager* manager;
        ReplenishStrategy* rs;
    public:
        DarkStore(string n, double x,double y){
            this->name = n;
            this->x=x;
            this->y=y;
            this->manager=new InventoryManager(new DBInventoryStore());
        }

        void setReplenishStrategy(ReplenishStrategy* rs){
            this->rs=rs;
        }

        
        int distanceTo(double ux, double uy){
            return sqrt((ux-x)*(ux-x)+(uy-y)*(uy-y));
        }
        void runReplenish(map<int,int> itemsToreplenish){
            if(rs)
            rs->replenish(manager,itemsToreplenish);
        }
        vector<Product*> getAllProducts(){
            return manager->getAvailableProducts();
        }
        void addStock(int sku, int q){
            manager->addStock(sku,q);
        }

        void removeStock(int sku, int q){
            manager->removeStock(sku,q);
        }

        int getStock(int sku){
            return manager->getStock(sku);
        }

        string getName(){
            return this->name;
        }
        double getXCoordinate(){
            return this->x;
        }
        double getYCoordinate(){
            return this->y;
        }
        InventoryManager* getManager(){
            return this->manager;
        }
};

class DarkStoreManager{
    private:
        static DarkStoreManager* instance;
        vector<DarkStore*>* darkStores;

        DarkStoreManager(){
            darkStores = new vector<DarkStore*>();
        }
    public:
        static DarkStoreManager* getInstance(){
            if(!instance){
                instance = new DarkStoreManager();
            }
            return instance;
        }
        ~DarkStoreManager() {
        for (auto ds : *darkStores) {
            delete ds;
        }
        delete darkStores;
    }

        void RegisterStore(DarkStore* d){
            darkStores->push_back(d);
        }

        vector<DarkStore*> getNearByStores(double ux, double uy, int maxDist){
            vector<pair<int,DarkStore*>> dist;

            for(auto it:*darkStores){
                int d = it->distanceTo(ux,uy);
                if(d<=maxDist) dist.push_back({d,it});
            }

            sort(dist.begin(),dist.end(),[](auto &a, auto &b){ return a.first < b.first; });
            vector<DarkStore*> result;
            for(auto it:dist){
                result.push_back(it.second);
            }
            return result;
        }
};

DarkStoreManager* DarkStoreManager::instance = nullptr;

class Cart{
    private:
        vector<pair<Product*,int>> items;
    public:
        ~Cart() {
            for (auto &it : items) {
                delete it.first;
            }
        }
        void addItem(int sku,int q){
            Product* prod = ProductFactory::createProduct(sku);
            items.push_back(make_pair(prod, q));
            cout << "[Cart] Added SKU " << sku << " (" << prod->getName() 
                << ") x" << q << endl;
            }
        double getTotal(){
            double total=0;
            for(auto item:items){
                total+=(item.first->getPrice()* item.second);
            }
            return total;
        }
        vector<pair<Product*,int>> getItems(){
            return items;
        }
};

class User{
    public:
        string name;
        double x,y;
        Cart* cart;
        User(string name,double x, double y){
            this->name=name;
            this->x=x;
            this->y=y;
            this->cart=new Cart();
        }
        ~User() {
            delete cart;
        }

        Cart* getCart() {
            return cart;
        }
};

class DeliveryPartner{
    public:
        string name;
        DeliveryPartner(string name){
            this->name=name;
        }
};

class Order{
    public:
        static int nextOrderId;
        int orderId;
        User* user;
        vector<pair<Product*,int>> items;
        vector<DeliveryPartner*> partners;
        double totalAmount;

        Order(User* u){
            orderId=nextOrderId++;
            user=u;
            totalAmount = 0.0;
        }
};

int Order::nextOrderId=0;

class OrderManager{
    private:
        static OrderManager* instance;
        vector<Order*>* orders;

        OrderManager(){
            orders = new vector<Order*>();
        }
    public:
        static OrderManager* getInstance(){
            if(!instance){
                instance = new OrderManager();
            }
            return instance;
        }

        ~OrderManager() {
            for (auto ord : *orders) {
                delete ord;
            }
            delete orders;
        }

        void placeOrder(User* user, Cart* cart) {
            cout << "\n[OrderManager] Placing Order for: " << user->name << "\n";

            // product --> Qty
            vector<pair<Product*,int>> requestedItems = cart->getItems();
        
            // 1) Find nearby dark stores within 5 KM
            double maxDist = 5.0;
            vector<DarkStore*> nearbyDarkStores = DarkStoreManager::getInstance()->getNearByStores(user->x, user->y, maxDist);
            
            if (nearbyDarkStores.empty()) {
                cout << "  No dark stores within 5 KM. Cannot fulfill order.\n";
                return;
            }
        
            Order* order = new Order(user);

            cout << "  Splitting order across stores...\n";

            map<int,int> allItems; //SKU --> Qty

            for (pair<Product*,int>& item : requestedItems) {
                allItems[item.first->getSku()] = item.second;
            }
    
            int partnerId = 1;
            for (DarkStore* store : nearbyDarkStores) {

                // If allItems becomes empty, we break early (all SKUs have been assigned)
                if (allItems.empty()) break;

                cout << "   Checking: " << store->getName() << "\n";
    
                bool assigned = false;
                vector<int> toErase;
                
                for (auto& [sku, qtyNeeded] : allItems) {

                    int availableQty = store->getStock(sku);
                    if (availableQty <= 0) continue;
    
                    //take whichever is smaller: available or qtyNeeded.
                    int takenQty = min(availableQty, qtyNeeded);
                    store->removeStock(sku, takenQty);

                    cout << "     " << store->getName() << " supplies SKU " << sku 
                        << " x" << takenQty << "\n";

                    order->items.push_back({ ProductFactory::createProduct(sku), takenQty });
    
                    // Adjust the Quantity
                    if (qtyNeeded > takenQty) {
                        allItems[sku] = qtyNeeded - takenQty;
                    } else {
                        toErase.push_back(sku);
                    }
                    assigned = true;
                }

                // After iterating all SKUs in allItems, we erase 
                // any fully‐satisfied SKUs from the allItems map.
                for (int sku : toErase) allItems.erase(sku);
    
                // If at least one SKU was taken from this store, we assign 
                // a new DeliveryPartner.
                if (assigned) {
                    string pname = "Partner" + to_string(partnerId++);
                    order->partners.push_back(new DeliveryPartner(pname));
                    cout << "     Assigned: " << pname << " for " << store->getName() << "\n";
                }
            }
    
            //  if remaining still has entries, we print which SKUs/quantities could not be fulfilled.
            if (!allItems.empty()) {
                cout << "  Could not fulfill:\n";
                for (auto& [sku, qty] : allItems) {
                    cout << "    SKU " << sku << " x" << qty << "\n";
                }
            }

            // recompute order->totalAmount
            double sum = 0;
            for (auto& item : order->items) {
                sum += item.first->getPrice() * item.second;
            }
            order->totalAmount = sum;
    
            // Printing Order Summary
            cout << "\n[OrderManager] Order #" << order->orderId << " Summary:\n";
            cout << "  User: " << user->name << "\n  Items:\n";
            for (auto& item : order->items) {
                cout << "    SKU " << item.first->getSku()
                    << " (" << item.first->getName() << ") x" << item.second
                    << " @ ₹" << item.first->getPrice() * item.second << "\n";
            }
            cout << "  Total: ₹" << order->totalAmount << "\n  Partners:\n";
            for (auto* dp : order->partners) {
                cout << "    " << dp->name << "\n";
            }
            cout << endl;
        
            orders->push_back(order);

            // Cleanups
            for (auto* dp : order->partners) delete dp;
            for (auto& item : order->items) delete item.first;
        }
        vector<Order*> getAllOrders() {
            return *orders;
        }
};

OrderManager* OrderManager::instance = nullptr;


class ZeptoHelper {
public:
    static void showAllItems(User* user) {
        cout << "\n[Zepto] All Available products within 5 KM for " << user->name << ":\n";

        DarkStoreManager* dsManager = DarkStoreManager::getInstance();
        vector<DarkStore*> nearbyStores = dsManager->getNearByStores(user->x, user->y, 5.0);

        // Collect each SKU → price (so we only display each product once)
        map<int, double> skuToPrice;
        map<int, string> skuToName;

        for (DarkStore* darkStore : nearbyStores) {
            vector<Product*> products = darkStore->getAllProducts();

            for (Product* product : products) {
                int sku = product->getSku();

                if (skuToPrice.count(sku) == 0) {
                    skuToPrice[sku] = product->getPrice();
                    skuToName[sku]  = product->getName();
                }
            }
        }

        for (auto& entry : skuToPrice) {
            int sku = entry.first;
            double price = entry.second;
            cout << "  SKU " << sku << " - " << skuToName[sku] << " @ ₹" << price << "\n";
        }
    }

    static void initialize() {
        auto dsManager = DarkStoreManager::getInstance();

        // DarkStore A.......
        DarkStore* darkStoreA = new DarkStore("DarkStoreA", 0.0, 0.0);
        darkStoreA->setReplenishStrategy(new ThresholdReplenish(3));
 
        cout << "\nAdding stocks in DarkStoreA...." << endl;  
        darkStoreA->addStock(101, 5); // Apple
        darkStoreA->addStock(102, 2); // Banana

        // DarkStore B.......
        DarkStore* darkStoreB = new DarkStore("DarkStoreB", 4.0, 1.0);
        darkStoreB->setReplenishStrategy(new ThresholdReplenish(3));

        cout << "\nAdding stocks in DarkStoreB...." << endl; 
        darkStoreB->addStock(101, 3); // Apple
        darkStoreB->addStock(103, 10); // Chocolate

        // DarkStore C.......
        DarkStore* darkStoreC = new DarkStore("DarkStoreC", 2.0, 3.0);
        darkStoreC->setReplenishStrategy(new ThresholdReplenish(3));

        cout << "\nAdding stocks in DarkStoreC...." << endl; 
        darkStoreC->addStock(102, 5); // Banana
        darkStoreC->addStock(201, 7); // T-Shirt

        dsManager->RegisterStore(darkStoreA);
        dsManager->RegisterStore(darkStoreB);
        dsManager->RegisterStore(darkStoreC);
    }
};

/////////////////////////////////////////////
// Main(): High-Level Flow
/////////////////////////////////////////////

int main() {

    // 1) Initialize.
    ZeptoHelper::initialize();

    // 2) A User comes on Platform
    User* user = new User("Aditya", 1.0, 1.0);
    cout <<"\nUser with name " << user->name<< " comes on platform" << endl;

    // 3) Show all available items via Zepto
    ZeptoHelper::showAllItems(user);

    // 4) User adds items to cart (some not in a single store)
    cout<<"\nAdding items to cart\n";
    Cart* cart = user->getCart();
    cart->addItem(101, 4);  // dsA has 5, dsB has 3 
    cart->addItem(102, 3);  // dsA has 2, dsC has 5
    cart->addItem(103, 2);  // dsB has 10

    // 5) Place Order
    OrderManager::getInstance()->placeOrder(user, user->cart);

    // 6) Cleanup
    delete user;
    delete DarkStoreManager::getInstance();  // deletes all DarkStores and their inventoryManagers

    return 0;
}
