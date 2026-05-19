#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// ================= OBSERVER INTERFACE =================

class ISubscriber {
public:
    virtual void update(
        const string& channelName,
        const string& videoTitle
    ) = 0;

    virtual string getName() const = 0;

    virtual ~ISubscriber() = default;
};

// ================= SUBJECT INTERFACE =================

class IChannel {
public:
    virtual void subscribe(ISubscriber* subscriber) = 0;

    virtual void unsubscribe(ISubscriber* subscriber) = 0;

    virtual void notifySubscribers() = 0;

    virtual ~IChannel() = default;
};

// ================= CONCRETE SUBJECT =================

class Channel : public IChannel {
private:
    string name;
    string latestVideo;

    vector<ISubscriber*> subscribers;

public:
    Channel(const string& name)
        : name(name) {}

    void subscribe(ISubscriber* subscriber) override {

        auto it = find(
            subscribers.begin(),
            subscribers.end(),
            subscriber
        );

        if (it == subscribers.end()) {

            subscribers.push_back(subscriber);

            cout << subscriber->getName()
                 << " subscribed to "
                 << name
                 << endl;
        }
    }

    void unsubscribe(ISubscriber* subscriber) override {

        auto it = find(
            subscribers.begin(),
            subscribers.end(),
            subscriber
        );

        if (it != subscribers.end()) {

            subscribers.erase(it);

            cout << subscriber->getName()
                 << " unsubscribed from "
                 << name
                 << endl;
        }
    }

    void notifySubscribers() override {

        // safe copy during iteration
        auto currentSubscribers = subscribers;

        for (auto subscriber : currentSubscribers) {

            subscriber->update(
                name,
                latestVideo
            );
        }
    }

    void uploadVideo(const string& title) {

        latestVideo = title;

        cout << "\n====================================\n";
        cout << name
             << " uploaded new video: "
             << title
             << endl;
        cout << "====================================\n";

        notifySubscribers();
    }
};

// ================= CONCRETE OBSERVER =================

class Subscriber : public ISubscriber {
private:
    string name;

public:
    Subscriber(const string& name)
        : name(name) {}

    void update(
        const string& channelName,
        const string& videoTitle
    ) override {

        cout << "Hey "
             << name
             << ", "
             << channelName
             << " uploaded: "
             << videoTitle
             << endl;
    }

    string getName() const override {
        return name;
    }
};

// ================= MAIN =================

int main() {

    // channels
    Channel coderArmy("CoderArmy");
    Channel techWorld("TechWorld");

    // subscribers
    Subscriber varun("Varun");
    Subscriber tarun("Tarun");
    Subscriber aman("Aman");

    // subscriptions
    coderArmy.subscribe(&varun);
    coderArmy.subscribe(&tarun);

    techWorld.subscribe(&varun);
    techWorld.subscribe(&aman);

    // upload videos
    coderArmy.uploadVideo(
        "Observer Pattern Tutorial"
    );

    techWorld.uploadVideo(
        "System Design Basics"
    );

    // unsubscribe
    coderArmy.unsubscribe(&tarun);

    // upload again
    coderArmy.uploadVideo(
        "Decorator Pattern Tutorial"
    );

    return 0;
}