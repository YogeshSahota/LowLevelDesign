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

enum class LogLevel{
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL
};

class LogMessage{
    private:
        string msg;
        LogLevel level;
        time_t timestamp;
    public:
        LogMessage(string msg, LogLevel level){
            this->msg=msg;
            this->level=level;
            this->timestamp=time(nullptr);
        }
        string getMsg(){
            return this->msg;
        }
        LogLevel getLogLevel(){
            return this->level;
        }
        time_t getTimestamp(){
            return this->timestamp;
        }
};

class Formatter{
    public:
        virtual string format(LogMessage* msg) = 0;
};

class SimpleFormatter:public Formatter{
    private:
        string logLevelToString(LogLevel level){
            switch (level)
            {
            case LogLevel::DEBUG :
                return "DEBUG";
            case LogLevel::INFO :
                return "INFO";
            case LogLevel::WARN :
                return "WARN";
            case LogLevel::ERROR :
                return "ERROR";
            case LogLevel::FATAL :
                return "FATAL";    
            default:
                return "UNKOWN";
            }
            return "UNKOWN";
        }
    public:
        string format(LogMessage* msg) override {
            string levelStr = logLevelToString(msg->getLogLevel());
            time_t ts = msg->getTimestamp();
            string timestamp = ctime(&ts);

            if(!timestamp.empty()) timestamp.pop_back();

            string msgStr = "[" + timestamp + "] [" + levelStr + "] " + msg->getMsg();

            return msgStr;
        }
};

class Appender{
    public:
        virtual void append(string msg) = 0;
};

class ConsoleAppender:public Appender{
    public:
        void append(string msg) override {
            cout << msg <<endl;
        };
};

class FileAppender:public Appender{
    private:
        ofstream file;
    public:
        FileAppender(string filename){
            file.open(filename,ios::app);
        }
        void append(string msg) override {
            if(file.is_open())
            file << msg <<endl;
        };
        ~FileAppender(){
            if(file.is_open()){
                file.close();
            }
        }
};

class Logger{
    private:
        vector<Appender*> appenders;
        Formatter* formatter;
        queue<LogMessage*> logQueue;
        mutex queueMutex;
        condition_variable cv;
        thread workerThread;
        bool stopWorker;

        Logger(){
            this->formatter=nullptr;
            this->stopWorker=false;
            this->workerThread = thread(&Logger::processLogs,this);
        }

        Logger(const Logger&) = delete;

        Logger& operator=(const Logger&) = delete;

        void processLogs(){
            while(true){
                unique_lock<mutex> lock(queueMutex);
                cv.wait(lock, [this] {return !logQueue.empty() || stopWorker;});
                cout << "logQueue" <<endl;
                while(!logQueue.empty()){
                    auto logMsg = logQueue.front();
                    logQueue.pop();
                    lock.unlock();
                    if(formatter){
                        string formattedMsg = formatter->format(logMsg);

                        for(auto &appender:appenders){
                            appender->append(formattedMsg);
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
        static Logger* getInstance(){
            static Logger* instance = new Logger();
            return instance;
        }

        void setFormatter(Formatter* f){
            this->formatter = f;
        }

        void addAppender(Appender* a){
            appenders.push_back(a);
        }

        void log(LogLevel level, string msg){
            {
                lock_guard<mutex> lock(queueMutex);
                logQueue.push(new LogMessage(msg,level));
            }
            cv.notify_one();
        }

        void shutdown(){
            {
                lock_guard<mutex> lock(queueMutex);
                stopWorker = true;
            }
            cv.notify_all();
            if(workerThread.joinable()){
                workerThread.join();
            }
        }

        ~Logger(){
            shutdown();
            delete formatter;

            for(auto &appender:appenders){
                delete appender;
            }
        }
};

int main() {

    Logger* logger =
        Logger::getInstance();

    logger->setFormatter(
        new SimpleFormatter());

    logger->addAppender(
        new ConsoleAppender());

    logger->addAppender(
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

                    logger->log(
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

    logger->log(
        LogLevel::ERROR,
        "Database Connection Failed");

    logger->log(
        LogLevel::FATAL,
        "System Crash");

    this_thread::sleep_for(
        chrono::seconds(1));

    logger->shutdown();

    return 0;
}