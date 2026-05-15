#include <iostream>

using namespace std;

class MySQLDatabase {
    public:
        void saveToSQL(string data) {
            cout << "Executing SQL Query: INSERT INTO users VALUES('" << data << "');" << endl;
        }
};

class MongoDBDatabase {
    public:
        void saveToMongo(string data) {
            cout << "Executing MongoDB Function: db.users.insert({name: '" << data << "'})" << endl;
        }
};

class UserService {
    private:
        MySQLDatabase sqlDb;
        MongoDBDatabase mongoDb;
    
    public:
        void storeUserToSQL(string user) {
            sqlDb.saveToSQL(user);
        }

        void storeUserToMongo(string user) {
            mongoDb.saveToMongo(user);
        }
};

int main() {
    UserService service;
    service.storeUserToSQL("Yogesh");
    service.storeUserToMongo("Sahota");
}