#include <iostream>
#include <mutex>

using namespace std;

class Singleton {
private:
    static Singleton* instance;
    static mutex mtx;

    Singleton() {
        cout << "Constructor called\n";
    }

public:
    static Singleton* getInstance() {
        lock_guard<mutex> lock(mtx);

        if (instance == nullptr) {
            instance = new Singleton();
        }

        return instance;
    }
};

Singleton* Singleton::instance = nullptr;
mutex Singleton::mtx;

int main() {
    Singleton* s1 = Singleton::getInstance();
    Singleton* s2 = Singleton::getInstance();

    cout << (s1 == s2) << endl;
}