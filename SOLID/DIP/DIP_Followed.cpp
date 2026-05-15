#include <iostream>

using namespace std;

class Persistence {
    public:
        virtual void save(string data) = 0;
};

class MySQLDatabase: public Persistence {
    public:
        void save(string data) {
            cout << "Executing SQL Query: INSERT INTO users VALUES('" << data << "');" << endl;
        }
};

class MongoDBDatabase: public Persistence {
    public:
        void save(string data) {
            cout << "Executing MongoDB Function: db.users.insert({name: '" << data << "'})" << endl;
        }
};

class UserService {
    private:
        Persistence* db;
    
    public:
        UserService(Persistence *db){
            this->db=db;
        }
        void storeUserToDb(string user) {
            db->save(user);
        }
};

int main() {
    Persistence* mongoDb = new MongoDBDatabase();
    Persistence* sqlDb = new MySQLDatabase();
    UserService service1(sqlDb);
    UserService service2(mongoDb);
    service1.storeUserToDb("Yogesh");
    service2.storeUserToDb("Sahota");
}