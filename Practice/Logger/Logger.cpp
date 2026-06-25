#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <ctime>
#include <chrono>

using namespace std;

//////////////////////////////////////////////////////////////
// Log Level
//////////////////////////////////////////////////////////////

enum class LogLevel {
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL
};

//////////////////////////////////////////////////////////////
// Log Message
//////////////////////////////////////////////////////////////

class LogMessage {
private:
    LogLevel level;
    string message;
    time_t timestamp;

public:
    LogMessage(
        LogLevel level,
        const string& message)
        : level(level),
          message(message),
          timestamp(time(nullptr)) {}

    LogLevel getLevel() const {
        return level;
    }

    const string& getMessage() const {
        return message;
    }

    time_t getTimestamp() const {
        return timestamp;
    }
};

//////////////////////////////////////////////////////////////
// Formatter Strategy
//////////////////////////////////////////////////////////////

class Formatter {
public:
    virtual string format(
        const LogMessage& msg) = 0;

    virtual ~Formatter() = default;
};

class SimpleFormatter : public Formatter {
private:

    string levelToString(
        LogLevel level) {

        switch(level) {

            case LogLevel::DEBUG:
                return "DEBUG";

            case LogLevel::INFO:
                return "INFO";

            case LogLevel::WARN:
                return "WARN";

            case LogLevel::ERROR:
                return "ERROR";

            case LogLevel::FATAL:
                return "FATAL";
        }

        return "UNKNOWN";
    }

public:

    string format(
        const LogMessage& msg)
        override {

        time_t ts =
            msg.getTimestamp();

        string timestamp =
            ctime(&ts);

        if(!timestamp.empty()) {
            timestamp.pop_back();
        }

        return "[" +
               timestamp +
               "] [" +
               levelToString(
                   msg.getLevel()) +
               "] " +
               msg.getMessage();
    }
};

//////////////////////////////////////////////////////////////
// Appender Strategy
//////////////////////////////////////////////////////////////

class Appender {
public:

    virtual void append(
        const string& msg) = 0;

    virtual ~Appender() = default;
};

class ConsoleAppender
    : public Appender {

public:

    void append(
        const string& msg)
        override {

        cout
            << msg
            << endl;
    }
};

class FileAppender
    : public Appender {

private:

    ofstream file;

public:

    FileAppender(
        const string& fileName) {

        file.open(
            fileName,
            ios::app);
    }

    void append(
        const string& msg)
        override {

        if(file.is_open()) {

            file
                << msg
                << endl;
        }
    }

    ~FileAppender() {

        if(file.is_open()) {

            file.close();
        }
    }
};

//////////////////////////////////////////////////////////////
// Logger
//////////////////////////////////////////////////////////////

class Logger {

private:

    vector<Appender*> appenders;

    Formatter* formatter;

    queue<LogMessage> logQueue;

    mutex queueMutex;

    condition_variable cv;

    thread workerThread;

    bool stopWorker;

private:

    Logger()
        : formatter(nullptr),
          stopWorker(false) {

        workerThread =
            thread(
                &Logger::processLogs,
                this);
    }

    Logger(
        const Logger&) = delete;

    Logger& operator=(
        const Logger&) = delete;

private:

    void processLogs() {

        while(true) {

            unique_lock<mutex> lock(queueMutex);

            cv.wait(lock, [this] { return stopWorker || !logQueue.empty();});

            while(!logQueue.empty()) {

                LogMessage msg =
                    logQueue.front();

                logQueue.pop();

                lock.unlock();

                if(formatter != nullptr) {

                    string formattedMsg =
                        formatter->format(msg);

                    for(Appender* appender
                        : appenders) {

                        appender->append(
                            formattedMsg);
                    }
                }

                lock.lock();
            }

            if(stopWorker &&
               logQueue.empty()) {

                break;
            }
        }
    }

public:

    static Logger& getInstance() {
        static Logger logger;
        return logger;
    }

    void setFormatter(
        Formatter* formatter) {

        this->formatter =
            formatter;
    }

    void addAppender(
        Appender* appender) {

        appenders.push_back(
            appender);
    }

    void log(
        LogLevel level,
        const string& message) {

        {
            lock_guard<mutex>
                lock(queueMutex);

            logQueue.push(
                LogMessage(
                    level,
                    message));
        }

        cv.notify_one();
    }

    void shutdown() {

        {
            lock_guard<mutex>
                lock(queueMutex);

            stopWorker = true;
        }

        cv.notify_all();

        if(workerThread.joinable()) {

            workerThread.join();
        }
    }

    ~Logger() {

        shutdown();

        delete formatter;

        for(Appender* appender
            : appenders) {

            delete appender;
        }
    }
};

//////////////////////////////////////////////////////////////
// Main
//////////////////////////////////////////////////////////////

int main() {

    Logger& logger =
        Logger::getInstance();

    logger.setFormatter(
        new SimpleFormatter());

    logger.addAppender(
        new ConsoleAppender());

    logger.addAppender(
        new FileAppender(
            "application.log"));

    vector<thread> threads;

    for(int i = 1;
        i <= 5;
        i++) {

        threads.emplace_back(
            [&logger, i]() {

                for(int j = 1;
                    j <= 5;
                    j++) {

                    logger.log(
                        LogLevel::INFO,
                        "Thread " +
                        to_string(i) +
                        " Message " +
                        to_string(j));
                }
            });
    }

    for(auto& t : threads) {

        t.join();
    }

    this_thread::sleep_for(
        chrono::seconds(1));

    logger.log(
        LogLevel::ERROR,
        "Database Connection Failed");

    logger.log(
        LogLevel::FATAL,
        "System Crash");

    this_thread::sleep_for(
        chrono::seconds(1));

    logger.shutdown();

    return 0;
}