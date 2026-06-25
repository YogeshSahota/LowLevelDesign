#include<iostream>
#include<ctime>
#include<atomic>
#include<unordered_map>
#include<mutex>

using namespace std;

class URL{
    private:
        string shortCode;
        string longUrl;
        time_t createdAt;
        time_t expiryTime;
        int clickCount;
    public:
        URL(string shortCode,string longUrl, time_t expiry = 0){
            this->clickCount=0;
            this->createdAt=time(nullptr);
            this->expiryTime=expiry;
            this->longUrl=longUrl;
            this->shortCode=shortCode;
        }

        string getShortCode(){
            return this->shortCode;
        }
        string getLongUrl(){
            return this->longUrl;
        }
        time_t getCreatedAt(){
            return this->createdAt;
        }
        time_t getExpiryTime(){
            return this->expiryTime;
        }
        int getClickCount(){
            return this->clickCount;
        }
        void incrementClick(){
            clickCount++;
        }
        bool isExpired(){
            if(expiryTime==0) return false;
            return time(nullptr)>expiryTime;
        }
};

class CodeGenerator{
    public:
        virtual string generateCode() = 0;
};

class Base62Generator:public CodeGenerator{
    private:
        atomic<long long> counter;
        string characters = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    public:
        Base62Generator(){
            counter=1;
        }

        string generateCode() override {
            long long number = counter++;
            string code = "";
            while(number>0){
                code = characters[number%62] + code;
                number/=62;
            }

            while(code.length()<6){
                code = "0"+code;
            }

            return code;
        }
};

class URLRepository{
    private:
        unordered_map<string,URL*> shortCodeToURL;
        unordered_map<string,string> longURLToShortCode;
        mutex repoMutex;
    public:
        bool shortCodeExists(const string &shortCode){
            lock_guard<mutex> lock(repoMutex);
            if(shortCodeToURL.find(shortCode)==shortCodeToURL.end()) return false;
            return true;
        }
        bool longURLExists(const string &longURL){
            lock_guard<mutex> lock(repoMutex);
            if(longURLToShortCode.find(longURL)==longURLToShortCode.end()) return false;
            return true;
        }

        void save(URL* url){
            lock_guard<mutex> lock(repoMutex);

            shortCodeToURL[url->getShortCode()]=url;
            longURLToShortCode[url->getLongUrl()]=url->getShortCode();
        }

        URL* findByShortCode(const string &shortCode){
            lock_guard<mutex> lock(repoMutex);
            if(shortCodeToURL.find(shortCode)==shortCodeToURL.end()) return nullptr;
            return shortCodeToURL[shortCode];
        }
        string getShortCode(const string &longURL){
            lock_guard<mutex> lock(repoMutex);
            if(longURLToShortCode.find(longURL)==longURLToShortCode.end()) return "";
            return longURLToShortCode[longURL];
        }
};

class AnalyticService{
    public:
        void recordClick(URL* url){
            if(url) url->incrementClick();
        }

        void printAnalytics(URL* url){
            if (url == nullptr)
                return;

            cout << "\nAnalytics\n";
            cout << "-----------\n";
            cout << "Short URL : "
                << url->getShortCode()
                << endl;

            cout << "Clicks    : "
                << url->getClickCount()
                << endl;
        }
};

class URLShortnerService{
    private:
        URLRepository* repository;
        CodeGenerator* codeGenerator;
        AnalyticService* analyticService;
    public:
        URLShortnerService(URLRepository* repository,CodeGenerator* codeGenerator,AnalyticService* analyticService){
            this->analyticService=analyticService;
            this->codeGenerator=codeGenerator;
            this->repository=repository;
        }

        string shortenURL(const string &longURL,const string customAlias="",int expiryInSec=0){
            if(repository->longURLExists(longURL)){
                return repository->getShortCode(longURL);
            }
            string shortCode = "";
            if(customAlias.size()){
                if(repository->shortCodeExists(customAlias)){
                    throw runtime_error("Custom Alias already exists!");
                }
                shortCode=customAlias;
            }else{
                do{
                    shortCode=codeGenerator->generateCode();
                }while(repository->shortCodeExists(shortCode));
            }

            time_t expiry = 0;

            if(expiryInSec>0){
                expiry=time(nullptr)+expiryInSec;
            }

            URL* url = new URL(shortCode,longURL,expiry);
            repository->save(url);
            return shortCode;
        }

        string getOriginalURL(string shortCode){
            if(!repository->shortCodeExists(shortCode)){
                throw runtime_error("Short URL doesn't exist!");
            }

            URL* url = repository->findByShortCode(shortCode);

            if(url->isExpired()){
                throw runtime_error("Expired short URL!");
            }
            analyticService->recordClick(url);
            return url->getLongUrl();
        }
        void showAnalytics(const string &shortCode){
            URL *url = repository->findByShortCode(shortCode);

            if (url == nullptr)
            {
                cout << "URL Not Found" << endl;
                return;
            }
            analyticService->printAnalytics(url);
        }
};

int main()
{

    URLRepository repository;

    Base62Generator generator;

    AnalyticService analytics;

    URLShortnerService service(
        &repository,
        &generator,
        &analytics);

    cout
        << "=========== URL SHORTENER ==========="
        << endl;

    //////////////////////////////////////////////////////////
    // Normal URL
    //////////////////////////////////////////////////////////

    string googleShortURL =
        service.shortenURL(
            "https://www.google.com");

    cout
        << "\nGoogle Short URL : "
        << googleShortURL
        << endl;

    //////////////////////////////////////////////////////////
    // Duplicate URL
    //////////////////////////////////////////////////////////

    string duplicate =
        service.shortenURL(
            "https://www.google.com");

    cout
        << "Duplicate returns : "
        << duplicate
        << endl;

    //////////////////////////////////////////////////////////
    // Custom Alias
    //////////////////////////////////////////////////////////

    string chatGPT =
        service.shortenURL(
            "https://chat.openai.com",
            "chatgpt");

    cout
        << "Custom Alias : "
        << chatGPT
        << endl;

    //////////////////////////////////////////////////////////
    // Redirect
    //////////////////////////////////////////////////////////

    cout
        << "\nRedirecting..."
        << endl;

    cout
        << service.getOriginalURL(
               googleShortURL)
        << endl;

    cout
        << service.getOriginalURL(
               googleShortURL)
        << endl;

    cout
        << service.getOriginalURL(
               chatGPT)
        << endl;

    //////////////////////////////////////////////////////////
    // Analytics
    //////////////////////////////////////////////////////////

    service.showAnalytics(
        googleShortURL);

    service.showAnalytics(
        chatGPT);
}