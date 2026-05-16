#include <iostream>

using namespace std;

class WalkableRobot {
    public:
        virtual void walk() = 0;
};

class NormalWalk: public WalkableRobot {
    public:
        virtual void walk() override {
            cout << "Walking\n";
        };
};


class NoWalk: public WalkableRobot {
    public:
        virtual void walk() override {
            cout << "Cannot walk\n";
        };
};

class FlyableRobot {
    public:
        virtual void fly() = 0;
};

class NormalFly: public FlyableRobot {
    public:
        virtual void fly() override {
            cout << "Flying\n";
        };
};


class NoFly: public FlyableRobot {
    public:
        virtual void fly() override {
            cout << "Cannot fly\n";
        };
};

class TalkableRobot {
    public:
        virtual void talk() = 0;
};

class NormalTalk: public TalkableRobot {
    public:
        virtual void talk() override {
            cout << "Talking\n";
        };
};


class NoTalk: public TalkableRobot {
    public:
        virtual void talk() override {
            cout << "Cannot talk\n";
        };
};

class Robot {
    private:
        WalkableRobot* w;
        TalkableRobot* t;
        FlyableRobot* f;
    
    public:
        Robot(WalkableRobot* w, TalkableRobot* t, FlyableRobot* f){
            this->w = w;
            this->t = t;
            this->f = f;
        }

        void walk(){
            w->walk();
        }

        void talk(){
            t->talk();
        }

        void fly(){
            f->fly();
        }
        virtual void projection() = 0;
};

// --- Concrete Robot Types ---
class CompanionRobot : public Robot {
public:
    CompanionRobot(WalkableRobot* w, TalkableRobot* t, FlyableRobot* f)
        : Robot(w, t, f) {}

    void projection() override {
        cout << "Displaying friendly companion features..." << endl;
    }
};

class WorkerRobot : public Robot {
public:
    WorkerRobot(WalkableRobot* w, TalkableRobot* t, FlyableRobot* f)
        : Robot(w, t, f) {}

    void projection() override {
        cout << "Displaying worker efficiency stats..." << endl;
    }
};

int main(){
    Robot* robot1 = new CompanionRobot(new NormalWalk(),new NormalTalk(),new NoFly());

    robot1->walk();
    robot1->talk();
    robot1->fly();
    robot1->projection();

    cout << "--------------------" << endl;

    Robot *robot2 = new WorkerRobot(new NoWalk(), new NoTalk(), new NormalFly());
    robot2->walk();
    robot2->talk();
    robot2->fly();
    robot2->projection();

    return 0;
}

