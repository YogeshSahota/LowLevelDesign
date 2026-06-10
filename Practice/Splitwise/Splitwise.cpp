#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <string>
#include <iomanip>

using namespace std;

enum class SplitType{
    EQUAL,
    EXACT,
    PERCENTAGE
};

class Split{
    public:
    string userId;
    double amount;
    Split(const string u, const double amt){
            userId=u;
            amount=amt;
        }
};

class SplitStrategy{
    public:
       virtual vector<Split> calcSplit(const double amt, const vector<string> &userIds, vector<double> values = {}) = 0;        
};

class EqualSplitStrategy: public SplitStrategy{
    public:
       vector<Split> calcSplit(const double amt, const vector<string> &userIds, vector<double> values = {}) override {
            vector<Split> splits;
            int n = userIds.size();
            double share = amt/n;
            for(int i=0;i<n;i++){
                splits.push_back(Split(userIds[i],share));
            }
            return splits;
       };        
};

class ExactSplitStrategy: public SplitStrategy{
    public:
       vector<Split> calcSplit(const double amt, const vector<string> &userIds, vector<double> values = {}) override {
            vector<Split> splits;
            int n = userIds.size();
            for(int i=0;i<n;i++){
                splits.push_back(Split(userIds[i],values[i]));
            }
            return splits;
       };        
};

class PercentageSplitStrategy: public SplitStrategy{
    public:
       vector<Split> calcSplit(const double amt, const vector<string> &userIds, vector<double> values = {}) override {
            vector<Split> splits;
            int n = userIds.size();
            for(int i=0;i<n;i++){
                double share = (values[i]/100)*amt;
                splits.push_back(Split(userIds[i],share));
            }
            return splits;
       };        
};

class SplitFactory {
public:
    static SplitStrategy* getSplitStrategy(SplitType type) {
        switch (type) {
            case SplitType::EQUAL:
                return new EqualSplitStrategy();
            case SplitType::EXACT:
                return new ExactSplitStrategy();
            case SplitType::PERCENTAGE:
                return new PercentageSplitStrategy();
            default:
                return new EqualSplitStrategy();
        }
    }
};

class Observer{
    public:
        virtual void update(const string &msg) = 0;
};

class User: public Observer{
    public:
    static int nextId;
    string userId;
    string name;
    string email;
    map<string,double> balances;
        User(string n, string e){
            userId = "user"+to_string(nextId++);
            name=n;
            email=e;
        }

        void update(const string &msg) override {
            cout << "[Notification] for " << name << " : " << msg << endl;
        }

        void updateBalance(const string &id, const double &amt){
            balances[id]+=amt;

            if(abs(balances[id])<0.01){
                balances.erase(id);
            }
        }

        double getTotalOwing(){
            double total = 0;

            for(auto balance:balances){
                if(balance.second<0){
                    total+=abs(balance.second);
                }
            }
            return total;
        }

        double getTotalOwed(){
            double total = 0;

            for(auto balance:balances){
                if(balance.second>0){
                    total+=abs(balance.second);
                }
            }
            return total;
        }
};

int User::nextId = 1;

class Expense {
public:
    static int nextExpenseId;
    string expenseId;
    string description;
    double totalAmount;
    string paidByUserId;
    vector<Split> splits;
    string groupId;
    
    Expense(const string& desc, double amount, const string& paidBy,
            vector<Split>& splits, const string group="") {
        this->expenseId = "expense" + std::to_string(++nextExpenseId);
        this->description = desc;
        this->totalAmount = amount;
        this->paidByUserId = paidBy;
        this->splits = splits;
        this->groupId = group;
    }
};
int Expense::nextExpenseId = 0;

class Group{
    private:
    User* getUserByuserId(string userId) {
        User* user = nullptr;

        for(User *member : members) {
            if(member->userId == userId) {
                user = member;
            }
        }
        return user;
    }
    public:
        static int nextGroupId;
        string groupId;
        string name;
        vector<User*> members;
        map<string,Expense*> groupExpenses;
        map<string,map<string,double>> groupBalances;

        Group(const string& name) {
            this->groupId = "group" + std::to_string(++nextGroupId);
            this->name = name;
        }
        
        ~Group() {
            // Clean up group's expenses
            for (auto& pair : groupExpenses) {
                delete pair.second;
            }
        }
        
        void addMember(User* user) {
            members.push_back(user);

            // Initialize balance map for new member
            groupBalances[user->userId] = map<string, double>();
            cout << user->name << " added to group " << name << endl;
        }

        bool removeMember(const string &userId){
            if(!canUserLeave(userId)){
                cout <<"\nUser not allowed to leave group without clearing expenses" << endl;
                return false;
            }

            for(auto *user:members){
                if(user->userId==userId){
                    members.erase(remove(members.begin(),members.end(),user),members.end());
                }
            }
            groupBalances.erase(userId);

            for(auto &memberBalances:groupBalances){
                memberBalances.second.erase(userId);
            }

            return true;
        }
        bool isMember(const string &userId){
            return groupBalances.find(userId)!=groupBalances.end();
        }
        bool canUserLeave(const string &userId){
            if(!isMember(userId)){
                throw runtime_error("User is not in group");
            }

            for(auto balance:groupBalances[userId]){
                if(abs(balance.second)>0) return false;
            }

            return true;
        }

        void notifyMembers(const string &msg){
            for(auto user:members){
                user->update(msg);
            }
        }

        void updateGroupBalance(const string& fromUserId, const string& toUserId, double amount) {
            groupBalances[fromUserId][toUserId]+=amount;
            groupBalances[toUserId][fromUserId]-=amount;

            if(abs(groupBalances[fromUserId][toUserId])<0.01){
                groupBalances[fromUserId].erase(toUserId);
            }
            if(abs(groupBalances[toUserId][fromUserId])<0.01){
                groupBalances[toUserId].erase(fromUserId);
            }
        }

        map<string, double> getUserGroupBalances(const string& userId) {
            if (!isMember(userId)) {
                throw runtime_error("user is not a part of this group");
            };
            return groupBalances[userId];
        }
        
        bool addExpense(const string &paidbyUserId, const string &description, SplitType type,
            const double &total, const vector<string> &userIds, vector<double> values = {}){
            if (!isMember(paidbyUserId)) {
                throw runtime_error("user is not a part of this group");
            };

            for(auto userId:userIds){
                if (!isMember(userId)) {
                    throw runtime_error("user is not a part of this group");
                };
            }

            vector<Split> splits = SplitFactory::getSplitStrategy(type)->calcSplit(total, userIds,values);

            Expense* expense = new Expense(description,total,paidbyUserId, splits, this->groupId);

            groupExpenses[paidbyUserId]=expense;

            for(auto split:splits){
                string paidToUserId = split.userId;
                double amt = split.amount;
                updateGroupBalance(paidbyUserId,paidToUserId,amt);
            }

            cout << endl << "=========== Sending Notifications ===================="<<endl;
            string paidByName = getUserByuserId(paidbyUserId)->name;
            notifyMembers("New expense added: " + description + " (Rs " + to_string(total) + ")");
            
            // Printing console message-------
            cout << endl << "=========== Expense Message ===================="<<endl;
            cout << "Expense added to " << name << ": " << description << " (Rs " << total 
                << ") paid by " << paidByName <<" and involved people are : "<< endl;
            if(!values.empty()) {
                for(int i=0; i<splits.size(); i++) {
                    cout << getUserByuserId(userIds[i])->name << " : " << values[i] << endl;
                }
            } 
            else {
                for(string user : userIds) {
                    cout << getUserByuserId(user)->name << ", ";
                }
                cout << endl << "Will be Paid Equally" << endl;
            }    
            //-----------------------------------
            
            return true;
        }

        bool settlePayment(string& fromUserId, string& toUserId, double amount) {
        // Validate that both users are group members
        if (!isMember(fromUserId) || !isMember(toUserId)) {
            cout << "user is not a part of this group" << endl;
            return false;
        }
        
        // Update group balances
        updateGroupBalance(fromUserId, toUserId, amount);
        
        // Get user names for display
        string fromName = getUserByuserId(fromUserId)->name;
        string toName = getUserByuserId(toUserId)->name;
        
        // Notify group members
        notifyMembers("Settlement: " + fromName + " paid " + toName + " Rs " + to_string(amount));
        
        cout << "Settlement in " << name << ": " << fromName << " settled Rs " 
             << amount << " with " << toName << endl;
        
        return true;
    }
    
    void showGroupBalances() {
        cout << "\n=== Group Balances for " << name << " ===" << endl;
        
        for (const auto& pair : groupBalances) {
            string memberId = pair.first;
            string memberName = getUserByuserId(memberId)->name;

            cout << memberName << "'s balances in group:" << endl;
            
            auto userBalances = pair.second;
            if (userBalances.empty()) {
                cout << "  No outstanding balances" << endl;
            } 
            else {
                for (const auto& userBalance : userBalances) {
                    string otherMemberUserId = userBalance.first;
                    string otherName = getUserByuserId(otherMemberUserId)->name;
                    
                    double balance = userBalance.second;
                    if (balance > 0) {
                        cout << "  " << otherName << " owes: Rs " << fixed << setprecision(2) << balance << endl;
                    } else {
                        cout << "  Owes " << otherName << ": Rs " << fixed << setprecision(2) << abs(balance) << endl;
                    }
                }
            }
        }
    }
};
int Group::nextGroupId = 0;

// Main ExpenseManager class (Singleton - Facade)
class Splitwise {
private:
    map<string, User*> users;
    map<string, Group*> groups;
    map<string, Expense*> expenses;

    static Splitwise* instance;
    Splitwise() {}
    
public:
    static Splitwise* getInstance() {
        if(instance == nullptr) {
            instance = new Splitwise();
        }
        return instance;
    }

    // User management
    User* createUser(string name, string email) {
        User* user = new User(name, email);
        users[user->userId] = user;
        cout << "User created: " << name << " (ID: " << user->userId << ")" << endl;
        return user;
    }
    
    User* getUser(const string& userId) {
        auto it = users.find(userId);
        return (it != users.end()) ? it->second : nullptr;
    }
    
    // Group management
    Group* createGroup(const string name) {
        Group* group = new Group(name);
        groups[group->groupId] = group;
        cout << "Group created: " << name << " (ID: " << group->groupId << ")" << endl;
        return group;
    }
    
    Group* getGroup(const string& groupId) {
        auto it = groups.find(groupId);
        return (it != groups.end()) ? it->second : nullptr;
    }
    
    void addUserToGroup(const string& userId, const string& groupId) {
        User* user = getUser(userId);
        Group* group = getGroup(groupId);
        
        if (user && group) {
            group->addMember(user);
        }
    }
    
    // Try to remove user from group - just delegates to group
    bool removeUserFromGroup(const string& userId, const string& groupId) {
        Group* group = getGroup(groupId);
        
        if (!group) {
            cout << "Group not found!" << endl;
            return false;
        }
        
        User* user = getUser(userId);
        if (!user) {
            cout << "User not found!" << endl;
            return false;
        }

        bool userRemoved = group->removeMember(userId);
        
        if(userRemoved) {
            cout << user->name << " successfully left " << group->name << endl;
        }
        return userRemoved;
    }
    
    // Expense management - delegate to group
    void addExpenseToGroup(string& groupId, string description, double amount, 
                          string& paidByUserId, vector<string>& involvedUsers, 
                          SplitType splitType, const vector<double>& splitValues = {}) {
        
        Group* group = getGroup(groupId);
        if (!group) {
            cout << "Group not found!" << endl;
            return;
        }

        group->addExpense(paidByUserId, description,splitType, amount, involvedUsers, splitValues);
    }
    
    // Settlement - delegate to group
    void settlePaymentInGroup(string& groupId, string& fromUserId, 
                              string& toUserId, double amount) {
        
        Group* group = getGroup(groupId);
        if (!group) {
            cout << "Group not found!" << endl;
            return;
        }
        
        group->settlePayment(fromUserId, toUserId, amount);
    }
    
    // Settlement
    void settleIndividualPayment(string& fromUserId, string& toUserId, double amount) {
        User* fromUser = getUser(fromUserId);
        User* toUser = getUser(toUserId);
        
        if (fromUser && toUser) {
            fromUser->updateBalance(toUserId, amount);
            toUser->updateBalance(fromUserId, -amount);
            
            cout << fromUser->name << " settled Rs" << amount << " with " << toUser->name << endl;
        }
    }
    
    void addIndividualExpense(string description, double amount, string paidByUserId,
                             string toUserId, SplitType splitType,
                            const vector<double>& splitValues = {}) {

        SplitStrategy* strategy = SplitFactory::getSplitStrategy(splitType);
        vector<Split> splits = strategy->calcSplit(amount, {paidByUserId, toUserId}, splitValues);

        Expense* expense = new Expense(description, amount, paidByUserId, splits);
        expenses[expense->expenseId] = expense;
        
        User* paidByUser = getUser(paidByUserId);
        User* toUser = getUser(toUserId);

        paidByUser->updateBalance(toUserId, amount);
        toUser->updateBalance(paidByUserId, -amount);
        
        cout << "Individual expense added: " << description << " (Rs " << amount 
                << ") paid by " << paidByUser->name <<" for " << toUser->name << endl;
    }
    
    // Display Method
    void showUserBalance(string& userId) {
        User* user = getUser(userId);
        if (!user) return;
        
        cout << endl << "=========== Balance for " << user->name <<" ===================="<<endl; 
        cout << "Total you owe: Rs " << fixed << setprecision(2) << user->getTotalOwed() << endl;
        cout << "Total others owe you: Rs " << fixed << setprecision(2) << user->getTotalOwing() << endl;
        
        cout << "Detailed balances:" << endl;
        for (auto& balance : user->balances) {
            User* otherUser = getUser(balance.first);
            if (otherUser) {
                if (balance.second > 0) {
                    cout << "  " << otherUser->name << " owes you: Rs" << balance.second << endl;
                } else {
                    cout << "  You owe " << otherUser->name << ": Rs" << abs(balance.second) << endl;
                }
            }
        }
    }
    
    void showGroupBalances(string& groupId) {
        Group* group = getGroup(groupId);
        if (!group) return;
        
        group->showGroupBalances();
    }
    
    // void simplifyGroupDebts(string& groupId) {
    //     Group* group = getGroup(groupId);
    //     if (!group) return;
                
    //     // Use group's balance data for debt simplification
    //     group->simplifyGroupDebts();
    // }
};

Splitwise* Splitwise::instance = nullptr;

int main() {
    
    Splitwise* manager = Splitwise::getInstance();
    
    cout << endl << "=========== Creating Users ===================="<<endl;
    User* user1 = manager->createUser("Aditya", "aditya@gmail.com");
    User* user2 = manager->createUser("Rohit", "rohit@gmail.com");
    User* user3 = manager->createUser("Manish", "manish@gmail.com");
    User* user4 = manager->createUser("Saurav", "saurav@gmail.com");
    
    cout << endl << "=========== Creating Group and Adding Members ===================="<<endl;
    Group* hostelGroup = manager->createGroup("Hostel Expenses");
    manager->addUserToGroup(user1->userId, hostelGroup->groupId);
    manager->addUserToGroup(user2->userId, hostelGroup->groupId);
    manager->addUserToGroup(user3->userId, hostelGroup->groupId);
    manager->addUserToGroup(user4->userId, hostelGroup->groupId);

    cout << endl << "=========== Adding Expenses in group ===================="<<endl;    
    vector<string> groupMembers = {user1->userId, user2->userId, user3->userId, user4->userId};
    manager->addExpenseToGroup(hostelGroup->groupId, "Lunch", 800.0, user1->userId, groupMembers, SplitType::EQUAL);
    
    vector<string> dinnerMembers = {user1->userId, user3->userId, user4->userId};
    vector<double> dinnerAmounts = {200.0, 300.0, 200.0};
    manager->addExpenseToGroup(hostelGroup->groupId, "Dinner", 700.0, user3->userId, dinnerMembers, 
                             SplitType::EXACT, dinnerAmounts);

    cout << endl << "=========== printing Group-Specific Balances ===================="<<endl; 
    manager->showGroupBalances(hostelGroup->groupId);

    // cout << endl << "=========== Debt Simplification ===================="<<endl; 
    // manager->simplifyGroupDebts(hostelGroup->groupId);

    cout << endl << "=========== printing Group-Specific Balances ===================="<<endl; 
    manager->showGroupBalances(hostelGroup->groupId);

    cout << endl << "=========== Adding Individual Expense ===================="<<endl; 
    manager->addIndividualExpense("Coffee", 40.0, user2->userId, user4->userId, SplitType::EQUAL);
    
    cout << endl << "=========== printing User Balances ===================="<<endl; 
    manager->showUserBalance(user1->userId);
    manager->showUserBalance(user2->userId);
    manager->showUserBalance(user3->userId);
    manager->showUserBalance(user4->userId);

    cout << endl << "==========Attempting to remove Rohit from group==========" << endl;
    manager->removeUserFromGroup(user2->userId, hostelGroup->groupId);

    cout << endl << "======== Making Settlement to Clear Rohit's Debt =========="<<endl; 
    manager->settlePaymentInGroup(hostelGroup->groupId, user2->userId, user3->userId, 200.0);
    
    cout << endl << "======== Attempting to Remove Rohit Again =========="<<endl;
    manager->removeUserFromGroup(user2->userId, hostelGroup->groupId);
    
    cout << endl << "=========== Updated Group Balances ===================="<<endl; 
    manager->showGroupBalances(hostelGroup->groupId);
    
    return 0;
}