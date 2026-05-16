#include <iostream>

using namespace std;

// Product
class Burger {
public:
    virtual void prepare() = 0;  // Pure virtual function
    virtual ~Burger() {}  // Virtual destructor
};

class BasicBurger : public Burger {
public:
    void prepare() override {
        cout << "Preparing Basic Burger with bun, patty, and ketchup!" << endl;
    }
};

class StandardBurger : public Burger {
public:
    void prepare() override {
        cout << "Preparing Standard Burger with bun, patty, cheese, and lettuce!" << endl;
    }
};

class PremiumBurger : public Burger {
public:
    void prepare() override {
        cout << "Preparing Premium Burger with gourmet bun, premium patty, cheese, lettuce, and secret sauce!" << endl;
    }
};

class BasicWheatBurger : public Burger {
public:
    void prepare() override {
        cout << "Preparing Basic Wheat Burger with Wheat bun, patty, and ketchup!" << endl;
    }
};

class StandardWheatBurger : public Burger {
public:
    void prepare() override {
        cout << "Preparing Standard Wheat Burger with Wheat bun, patty, cheese, and lettuce!" << endl;
    }
};

class PremiumWheatBurger : public Burger {
public:
    void prepare() override {
        cout << "Preparing Premium Wheat Burger with gourmet Wheat bun, premium patty, cheese, lettuce, and secret sauce!" << endl;
    }
};

class GarlicBread {
public:
    virtual void prepare() = 0;  // Pure virtual function
};

class BasicGarlicBread : public GarlicBread {
public:
    void prepare() override {
        cout << "Preparing Basic Garlic Bread!" << endl;
    }
};

class CheeseGarlicBread : public GarlicBread {
public:
    void prepare() override {
        cout << "Preparing Cheese Garlic Bread" << endl;
    }
};

class BasicWheatGarlicBread : public GarlicBread {
public:
    void prepare() override {
        cout << "Preparing Basic Wheat Garlic Bread!" << endl;
    }
};

class CheeseWheatGarlicBread : public GarlicBread {
public:
    void prepare() override {
        cout << "Preparing Cheese Wheat Garlic Bread" << endl;
    }
};

class MealFactory{
    public:
        virtual Burger* createBurger(string &type) = 0;
        virtual GarlicBread* createGarlicBread(string &type) = 0;
};

class SinghBurger: public MealFactory {
public:
    Burger* createBurger(string& type) {
        if (type == "basic") {
            return new BasicBurger();
        } else if (type == "standard") {
            return new StandardBurger();
        } else if (type == "premium") {
            return new PremiumBurger();
        } else {
            cout << "Invalid burger type! " << endl;
            return nullptr;
        }
    }

    GarlicBread* createGarlicBread(string& type) {
        if (type == "basic") {
            return new BasicGarlicBread();
        } else if (type == "cheese") {
            return new CheeseGarlicBread();
        }else {
            cout << "Invalid burger type! " << endl;
            return nullptr;
        }
    }
};

class KingBurger: public MealFactory {
public:
    Burger* createBurger(string& type) {
        if (type == "basic") {
            return new BasicWheatBurger();
        } else if (type == "standard") {
            return new StandardWheatBurger();
        } else if (type == "premium") {
            return new PremiumWheatBurger();
        } else {
            cout << "Invalid burger type! " << endl;
            return nullptr;
        }
    }

    GarlicBread* createGarlicBread(string& type) {
        if (type == "basic") {
            return new BasicWheatGarlicBread();
        } else if (type == "cheese") {
            return new CheeseWheatGarlicBread();
        }else {
            cout << "Invalid burger type! " << endl;
            return nullptr;
        }
    }
};

int main() {
    string burgerType = "basic";
    string garlicBreadType = "cheese";

    MealFactory* mealFactory = new KingBurger();

    Burger* burger = mealFactory->createBurger(burgerType);
    GarlicBread* garlicBread = mealFactory->createGarlicBread(garlicBreadType);

    burger->prepare();
    garlicBread->prepare();

    return 0;
}


