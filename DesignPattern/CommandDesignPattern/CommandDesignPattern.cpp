#include <iostream>
#include <vector>

using namespace std;

class Command {
    public:
        virtual void execute() = 0;
        virtual void undo() = 0;
};

class Fan {
    public:
        void on(){
            cout << "Fan is on" << endl;
        }
        void off(){
            cout << "Fan is off" << endl;
        }
};

class Light {
    public:
        void on(){
            cout << "Light is on" << endl;
        }
        void off(){
            cout << "Light is off" << endl;
        }
};

class LightCommand:public Command{
    private:
        Light* light;
    public:
        LightCommand(Light* l){
            this->light=l;
        }
        void execute() override {
            light->on();
        }
        void undo() override {
            light->off();
        }
};

class FanCommand:public Command{
    private:
        Fan* fan;
    public:
        FanCommand(Fan* l){
            this->fan=l;
        }
        void execute() override {
            fan->on();
        }
        void undo() override {
            fan->off();
        }
};

class RemoteController {
    private:
        static const int numButtons = 4;
        Command* buttons[numButtons];
        bool buttonPressed[numButtons];

    public:
        RemoteController(){
            for(int i=0;i<numButtons;i++){
                buttons[i]=nullptr;
                buttonPressed[i]=false;
            }
        }

        void setCommand(Command* c, int idx){
            if(buttons[idx]!=nullptr){
                delete buttons[idx];
            }
            buttons[idx]=c;
        }

        void pressButton(int idx){
            if(buttons[idx]!=nullptr){
                if(buttonPressed[idx]){
                    buttons[idx]->undo();
                }else{
                    buttons[idx]->execute();
                }
                buttonPressed[idx]=!buttonPressed[idx];
            }else{
                cout << "not assigned" <<endl;
            }
        }
};

int main(){
    Light* light = new Light();
    LightCommand* lCommand = new LightCommand(light);
    Fan* fan = new Fan();
    FanCommand* fCommand = new FanCommand(fan);

    RemoteController* remote = new RemoteController();

    remote->setCommand(lCommand,0);
    remote->setCommand(fCommand,1);

    remote->pressButton(0);
    remote->pressButton(0);
    remote->pressButton(1);
    remote->pressButton(1);
    remote->pressButton(2);

}