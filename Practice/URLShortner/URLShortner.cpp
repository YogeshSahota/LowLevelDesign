#include <iostream>
#include <unordered_map>
#include <string>
#include <atomic>
#include <mutex>
#include <ctime>
#include <vector>
#include <thread>
#include <chrono>
#include <stdexcept>

using namespace std;

class URL
{
private:
    string longURL;
    string shortCode;
    time_t createdAt;
    time_t expiryTime;
    int clickCount;

public:
    URL() {}

    URL(const string &longURL,
        const string &shortCode,
        time_t expiryTime = 0)
        : longURL(longURL),
          shortCode(shortCode),
          createdAt(time(nullptr)),
          expiryTime(expiryTime),
          clickCount(0) {}

    string getLongURL() const
    {
        return longURL;
    }

    string getShortCode() const
    {
        return shortCode;
    }

    time_t getCreatedTime() const
    {
        return createdAt;
    }

    time_t getExpiryTime() const
    {
        return expiryTime;
    }

    int getClickCount() const
    {
        return clickCount;
    }

    void incrementClickCount()
    {
        clickCount++;
    }

    bool isExpired() const
    {

        if (expiryTime == 0)
            return false;

        return time(nullptr) > expiryTime;
    }
};

//////////////////////////////////////////////////////////////
// Strategy Pattern
//////////////////////////////////////////////////////////////

class CodeGenerator
{
public:
    virtual string generateCode() = 0;

    virtual ~CodeGenerator() {}
};

//////////////////////////////////////////////////////////////
// Base62 Generator
//////////////////////////////////////////////////////////////

class Base62Generator : public CodeGenerator
{

private:
    atomic<long long> counter;

    string characters;

public:
    Base62Generator()
        : counter(1),
          characters(
              "0123456789"
              "abcdefghijklmnopqrstuvwxyz"
              "ABCDEFGHIJKLMNOPQRSTUVWXYZ")
    {
    }

    string generateCode() override
    {

        long long number = counter++;

        string code;

        while (number > 0)
        {

            code =
                characters[number % 62] +
                code;

            number /= 62;
        }

        while (code.length() < 6)
            code = "0" + code;

        return code;
    }
};

//////////////////////////////////////////////////////////////
// Repository
//////////////////////////////////////////////////////////////

class URLRepository
{

private:
    unordered_map<
        string,
        URL>
        shortToURL;

    unordered_map<
        string,
        string>
        longToShort;

    mutex repoMutex;

public:
    bool shortCodeExists(
        const string &shortCode)
    {

        lock_guard<mutex> lock(repoMutex);

        return shortToURL.find(shortCode) != shortToURL.end();
    }

    bool longURLExists(
        const string &longURL)
    {

        lock_guard<mutex> lock(repoMutex);

        return longToShort.find(longURL) != longToShort.end();
    }

    void save(
        const URL &url)
    {

        lock_guard<mutex> lock(repoMutex);

        shortToURL[url.getShortCode()] = url;

        longToShort[url.getLongURL()] =
            url.getShortCode();
    }

    URL *findByShortCode(
        const string &shortCode)
    {

        lock_guard<mutex> lock(repoMutex);

        auto it =
            shortToURL.find(shortCode);

        if (it == shortToURL.end())
            return nullptr;

        return &(it->second);
    }

    string getShortCode(
        const string &longURL)
    {

        lock_guard<mutex> lock(repoMutex);

        auto it =
            longToShort.find(longURL);

        if (it == longToShort.end())
            return "";

        return it->second;
    }
};

//////////////////////////////////////////////////////////////
// Analytics Service
//////////////////////////////////////////////////////////////

class AnalyticsService
{

public:
    void recordClick(
        URL *url)
    {

        if (url)
            url->incrementClickCount();
    }

    void printAnalytics(
        URL *url)
    {

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

//////////////////////////////////////////////////////////////
// URL Shortener Service
//////////////////////////////////////////////////////////////

class URLShortenerService
{

private:
    URLRepository *repository;

    CodeGenerator *generator;

    AnalyticsService *analytics;

public:
    URLShortenerService(
        URLRepository *repository,
        CodeGenerator *generator,
        AnalyticsService *analytics)
        : repository(repository),
          generator(generator),
          analytics(analytics)
    {
    }
    //////////////////////////////////////////////////////////
    // Create Short URL
    //////////////////////////////////////////////////////////

    string shortenURL(
        const string &longURL,
        const string &customAlias = "",
        int expiryInSeconds = 0)
    {

        // Return existing short URL if already present
        if (repository->longURLExists(longURL))
        {

            return repository->getShortCode(longURL);
        }

        string shortCode;

        //////////////////////////////////////////////////////
        // Custom Alias
        //////////////////////////////////////////////////////

        if (!customAlias.empty())
        {

            if (repository->shortCodeExists(customAlias))
            {

                throw runtime_error(
                    "Custom Alias Already Exists");
            }

            shortCode = customAlias;
        }
        else
        {

            //////////////////////////////////////////////////
            // Collision Handling
            //////////////////////////////////////////////////

            do
            {

                shortCode =
                    generator->generateCode();

            } while (repository->shortCodeExists(shortCode));
        }

        //////////////////////////////////////////////////////
        // Expiry
        //////////////////////////////////////////////////////

        time_t expiryTime = 0;

        if (expiryInSeconds > 0)
        {

            expiryTime =
                time(nullptr) +
                expiryInSeconds;
        }

        URL url(
            longURL,
            shortCode,
            expiryTime);

        repository->save(url);

        return shortCode;
    }

    //////////////////////////////////////////////////////////
    // Redirect
    //////////////////////////////////////////////////////////

    string getOriginalURL(
        const string &shortCode)
    {

        URL *url =
            repository->findByShortCode(
                shortCode);

        if (url == nullptr)
        {

            throw runtime_error(
                "Short URL Not Found");
        }

        if (url->isExpired())
        {

            throw runtime_error(
                "URL Expired");
        }

        analytics->recordClick(url);

        return url->getLongURL();
    }

    //////////////////////////////////////////////////////////
    // Analytics
    //////////////////////////////////////////////////////////

    void showAnalytics(
        const string &shortCode)
    {

        URL *url =
            repository->findByShortCode(
                shortCode);

        if (url == nullptr)
        {

            cout
                << "URL Not Found"
                << endl;

            return;
        }

        analytics->printAnalytics(url);
    }
};

//////////////////////////////////////////////////////////////
// Main
//////////////////////////////////////////////////////////////

int main()
{

    URLRepository repository;

    Base62Generator generator;

    AnalyticsService analytics;

    URLShortenerService service(
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