#include <iostream>
#include <vector>
#include <map>
#include <time.h>
#include <deque>

using namespace std;

class IObserver{
    public:
        virtual void update(const string &msg) = 0;
};

class SnakeAndLadderConsoleNotifier:public IObserver{
    public:
        void update(const string &msg) override{
            cout << "[Notification]: "<<msg<<endl;
        };
};

class Dice{
    private:
        int faces;
    public:
        Dice(int faces){
            this->faces=faces;
            srand(time(0));
        }

        int roll(){
            return rand()%faces+1;
        }
};

class BoardEntity{
    protected:
        int startPosition;
        int endPosition;
    public:
        BoardEntity(int start, int end){
            this->startPosition=start;
            this->endPosition=end;
        }

        int getStart(){
            return this->startPosition;
        }
        int getEnd(){
            return this->endPosition;
        }

        virtual string name() = 0;
        virtual void display() = 0;
};

class Snake: public BoardEntity{
    public:
        Snake(int start,int end):BoardEntity(start,end){
            if(end>=start){
                cout << "Invalid snake entity!"<<endl;
            }
        }
        void display() override {
            cout << "Snake: " << startPosition << " -> " << endPosition << endl;
        }

        string name() override {
            return "SNAKE" ;
        }
};

class Ladder: public BoardEntity{
    public:
        Ladder(int start,int end):BoardEntity(start,end){
            if(start>=end){
                cout << "Invalid ladder entity!"<<endl;
            }
        }
        void display() override {
            cout << "Ladder: " << startPosition << " -> " << endPosition << endl;
        }

        string name() override {
            return "LADDER" ;
        }
};

class BoardSetupStrategy;

class Board{
    private:
        int size;
        map<int,BoardEntity*> boardEntities;
    public:
        Board(int s){
            this->size=s*s;
        }

        bool canAddEntity(int start){
            return boardEntities.find(start)==boardEntities.end();
        }

        void addBoardEntity(BoardEntity* entity){
            if(canAddEntity(entity->getStart()))
                boardEntities[entity->getStart()]=entity;
        }

        void setupBoard(BoardSetupStrategy* strategy);

        int getSize(){
            return this->size;
        }

        BoardEntity* getEntity(int pos){
            if(boardEntities.find(pos)!=boardEntities.end()){
                return boardEntities[pos];
            }
            return nullptr;
        }

        void display(){
            cout << "\n=== Board Configuration ===" <<endl;
            cout << "Board Size: "<<size<<endl;

            int ladderCount=0;
            int snakeCount=0;

            for(auto it:boardEntities){
                if(it.second->name()=="SNAKE") snakeCount++;
                else ladderCount++;
            }

            cout << "Snakes: "<<snakeCount<<endl;
            for(auto it:boardEntities){
                if(it.second->name()=="SNAKE") it.second->display();
            }

            cout << "Ladders: "<<ladderCount<<endl;
            for(auto it:boardEntities){
                if(it.second->name()=="LADDER") it.second->display();
            }

            cout << "=========================" << endl;
        }

        ~Board(){
            for(auto it:boardEntities){
                delete it.second;
            }
        }
};

class BoardSetupStrategy{
    public:
        virtual void setupBoard(Board* board) = 0;
};

class RandomBoardStrategy:public BoardSetupStrategy{
    public:
        enum Difficulty{
            EASY,
            MEDIUM,
            HARD
        };
    private:
        Difficulty difficulty;

        void setupWithProbability(Board* board, double snakesProbability){
            cout <<"setupBoard" << endl;
            int boardSize = board->getSize();
            int totalEntities = boardSize/10;

            while(totalEntities){
                double prob = (double)rand()/RAND_MAX;

                if(prob<snakesProbability){
                    int start = rand()%(boardSize-10)+10;
                    int end = rand()%(start-1)+1;
                    int attempts = 1;
                    while(attempts<=50){
                        if(board->canAddEntity(start)){
                            board->addBoardEntity(new Snake(start,end));
                            break;
                        }
                        attempts++;
                    }
                }else{
                    int attempts = 0;
                    while(attempts < 50) {
                        int start = rand() % (boardSize - 10) + 1;
                        int end = rand() % (boardSize - start) + start + 1;
                        
                        if(board->canAddEntity(start) && end < boardSize) {
                            board->addBoardEntity(new Ladder(start, end));
                            break;
                        }
                        attempts++;
                    }
                }
                totalEntities--;
            }
        }
    public:
        RandomBoardStrategy(Difficulty difficulty){
            this->difficulty=difficulty;
        }

        void setupBoard(Board* board) override{
            cout <<"setupBoard" << endl;
            switch (difficulty)
            {
            case EASY:
                setupWithProbability(board,0.3);
                break;
            case MEDIUM:
                setupWithProbability(board,0.5);
                break;
            case HARD:
                setupWithProbability(board,0.7);
                break;       
            }
        }
};

class CustomSetupStrategy:public BoardSetupStrategy{
    private:
        int numLadders;
        int numSnakes;
        bool randomPositions;
        vector<pair<int,int>> snakePositions;
        vector<pair<int,int>> ladderPositions;
    
    public:
        CustomSetupStrategy(int numLadders,int numSnakes, bool randomPositions){
            this->numLadders=numLadders;
            this->numSnakes=numSnakes;
            this->randomPositions=randomPositions;
        }

        void addSnakePosition(int start, int end) {
            snakePositions.push_back(make_pair(start, end));
        }
        
        void addLadderPosition(int start, int end) {
            ladderPositions.push_back(make_pair(start, end));
        }

        void setupBoard(Board* board){
            if(randomPositions){
                int boardSize = board->getSize();

                int snakesAdded = 0;
                while(snakesAdded<numSnakes){
                    int start = rand()%(boardSize-10)+10;
                    int end = rand()%(start-1)+1;
                    if(board->canAddEntity(start)){
                        board->addBoardEntity(new Snake(start,end));
                        snakesAdded++;
                    }
                }

                int laddersAdded = 0;
                while(laddersAdded<numLadders){
                    int start = rand() % (boardSize - 10) + 1;
                    int end = rand() % (boardSize - start) + start + 1;
                    if(board->canAddEntity(start)&& end < boardSize){
                        board->addBoardEntity(new Ladder(start,end));
                        laddersAdded++;
                    }
                }

            }else{

                for(auto it:snakePositions){
                    if(board->canAddEntity(it.first)) board->addBoardEntity(new Snake(it.first,it.second));
                }

                for(auto it:ladderPositions){
                    if(board->canAddEntity(it.first)) board->addBoardEntity(new Ladder(it.first,it.second));
                }
            }
        }
};

// Standard Board Strategy - Traditional Snake & Ladder positions
class StandardBoardSetupStrategy : public BoardSetupStrategy {
public:
    void setupBoard(Board* board) override {
        // Only works for 10x10 board (100 cells)
        if(board->getSize() != 100) {
            cout << "Standard setup only works for 10x10 board!" << endl;
            return;
        }
        
        // Standard snake positions (based on traditional board)
        board->addBoardEntity(new Snake(99, 54));
        board->addBoardEntity(new Snake(95, 75));
        board->addBoardEntity(new Snake(92, 88));
        board->addBoardEntity(new Snake(89, 68));
        board->addBoardEntity(new Snake(74, 53));
        board->addBoardEntity(new Snake(64, 60));
        board->addBoardEntity(new Snake(62, 19));
        board->addBoardEntity(new Snake(49, 11));
        board->addBoardEntity(new Snake(46, 25));
        board->addBoardEntity(new Snake(16, 6));
        
        // Standard ladder positions
        board->addBoardEntity(new Ladder(2, 38));
        board->addBoardEntity(new Ladder(7, 14));
        board->addBoardEntity(new Ladder(8, 31));
        board->addBoardEntity(new Ladder(15, 26));
        board->addBoardEntity(new Ladder(21, 42));
        board->addBoardEntity(new Ladder(28, 84));
        board->addBoardEntity(new Ladder(36, 44));
        board->addBoardEntity(new Ladder(51, 67));
        board->addBoardEntity(new Ladder(71, 91));
        board->addBoardEntity(new Ladder(78, 98));
        board->addBoardEntity(new Ladder(87, 94));
    }
};

void Board::setupBoard(BoardSetupStrategy* strategy){
    strategy->setupBoard(this);
}

class SnakeAndLadderPlayer {
private:
    int playerId;
    string name;
    int position;
    int score;
    
public:
    SnakeAndLadderPlayer(int playerId, string n) {
        this->playerId = playerId;
        name = n;
        position = 0;
        score = 0;
    }
    
    // Getters and Setters
    string getName() { 
        return name;
    }
    int getPosition() { 
        return position; 
    }
    void setPosition(int pos) { 
        position = pos; 
    }
    int getScore() { 
        return score;
    }
    void incrementScore() { 
        score++; 
    }
};

class SnakesAndLaddersRules{
    public:
        virtual bool isValidMove(int currentPos, int diceVal, int boardSize) = 0;
        virtual int calculateNewPos(int currentPos, int diceVal, Board* board) = 0;
        virtual bool checkWin(int pos, int boardSize) = 0;
};

class StandardSnakesAndLaddersRules: public SnakesAndLaddersRules{
    public:
        bool isValidMove(int currentPos, int diceVal, int boardSize) override{
            return (currentPos+diceVal)<=boardSize;
        };

        int calculateNewPos(int currentPos, int diceVal, Board* board) override{
            currentPos+=diceVal;
            BoardEntity* entity = board->getEntity(currentPos);
            if(entity){
                currentPos=entity->getEnd();
            }
            return currentPos;
        } 
        
        bool checkWin(int pos, int boardSize) override{
            return pos==boardSize;
        }
};

class SnakeAndLadderGame {
private:
    Board* board;
    Dice* dice;
    deque<SnakeAndLadderPlayer*> players;
    SnakesAndLaddersRules* rules;
    vector<IObserver*> observers;
    bool gameOver;
    
public:
    SnakeAndLadderGame(Board* b, Dice* d) {
        board = b;
        dice = d;
        rules = new StandardSnakesAndLaddersRules();
        gameOver = false;
    }
    
    void addPlayer(SnakeAndLadderPlayer* player) {
        players.push_back(player);
    }
    
    void addObserver(IObserver* observer) {
        observers.push_back(observer);
    }

    void notify(string msg) {
        for(auto observer : observers) {
            observer->update(msg);
        }
    }
    
    void displayPlayerPositions() {
        cout << "\n=== Current Positions ===" << endl;
        for(auto player : players) {
            cout << player->getName() << ": " << player->getPosition() << endl;
        }
        cout << "======================" << endl;
    }
    
    void play() {
        if(players.size() < 2) {
            cout << "Need at least 2 players!" << endl;
            return;
        }
        
        notify("Game started");

        board->display();
        
        while(!gameOver) {
            SnakeAndLadderPlayer* currentPlayer = players.front();
            
            cout << "\n" << currentPlayer->getName() << "'s turn. Press Enter to roll dice...";
            cin.ignore();
            cin.get();
            
            int diceValue = dice->roll();
            cout << "Rolled: " << diceValue << endl;
            
            int currentPos = currentPlayer->getPosition();
            
            if(rules->isValidMove(currentPos, diceValue, board->getSize())) {
                int intermediatePos = currentPos + diceValue;
                int newPos = rules->calculateNewPos(currentPos, diceValue, board);
                
                currentPlayer->setPosition(newPos);
                
                // Check if player encountered snake or ladder
                BoardEntity* entity = board->getEntity(intermediatePos);
                if(entity != nullptr) {
                    bool isSnake = (entity->name() == "SNAKE");
                    if(isSnake) {
                        cout << "Oh no! Snake at " << intermediatePos << "! Going down to " << newPos << endl;
                        notify(currentPlayer->getName() + " encountered snake at " + to_string(intermediatePos) + " now going down to " + to_string(newPos));
                    }
                    else {
                        cout << "Great! Ladder at " << intermediatePos << "! Going up to " << newPos << endl;
                        notify(currentPlayer->getName() + " encountered ladder at " + to_string(intermediatePos) + " now going up to " + to_string(newPos));
                    }
                }
                
                notify(currentPlayer->getName() + " played. New Position : " + to_string(newPos));
                displayPlayerPositions();
                
                if(rules->checkWin(newPos, board->getSize())) {
                    cout << "\n" << currentPlayer->getName() << " wins!" << endl;
                    currentPlayer->incrementScore();

                    notify("Game Ended. Winner is : " + currentPlayer->getName());
                    gameOver = true;
                }
                else {
                    // Move player to back of queue
                    players.pop_front();
                    players.push_back(currentPlayer);
                }
            }
            else {
                cout << "Need exact roll to reach " << board->getSize() << "!" << endl;
                // Move player to back of queue
                players.pop_front();
                players.push_back(currentPlayer);
            }
        }
    }
    
    ~SnakeAndLadderGame() {
        delete rules;
    }
};

// Factory Pattern
class SnakeAndLadderGameFactory {
public:
    static SnakeAndLadderGame* createStandardGame() {
        Board* board = new Board(10);  // Standard 10x10 board
        BoardSetupStrategy* strategy = new StandardBoardSetupStrategy();
        board->setupBoard(strategy);
        delete strategy;
        
        Dice* dice = new Dice(6);  // Standard 6-faced dice
        
        return new SnakeAndLadderGame(board, dice);
    }
    
    static SnakeAndLadderGame* createRandomGame(int boardSize, RandomBoardStrategy::Difficulty difficulty) {
        cout << "createRandomGame "<<endl;
        Board* board = new Board(boardSize);
        BoardSetupStrategy* strategy = new RandomBoardStrategy(difficulty);
        board->setupBoard(strategy);
        delete strategy;
        
        Dice* dice = new Dice(6);
        cout <<"Board created" <<endl;
        return new SnakeAndLadderGame(board, dice);
    }
    
    static SnakeAndLadderGame* createCustomGame(int boardSize, BoardSetupStrategy* strategy) {
        Board* board = new Board(boardSize);
        board->setupBoard(strategy);
        
        Dice* dice = new Dice(6);
        
        return new SnakeAndLadderGame(board, dice);
    }
};

// Main function for Snake and Ladder
int main() {
    cout << "=== SNAKE AND LADDER GAME ===" << endl;
    
    SnakeAndLadderGame* game = nullptr;
    Board* board = nullptr;
    
    cout << "Choose game setup:" << endl;
    cout << "1. Standard Game (10x10 board with traditional positions)" << endl;
    cout << "2. Random Game with Difficulty" << endl;
    cout << "3. Custom Game" << endl;
    
    int choice;
    cin >> choice;
    
    if(choice == 1) {
        // Standard game
        game = SnakeAndLadderGameFactory::createStandardGame();
        
        
    }
    else if(choice == 2) {
        // Random game with difficulty
        int boardSize;
        cout << "Enter board size (e.g., 10 for 10x10 board): ";
        cin >> boardSize;
        
        cout << "Choose difficulty:" << endl;
        cout << "1. Easy (more ladders)" << endl;
        cout << "2. Medium (balanced)" << endl;
        cout << "3. Hard (more snakes)" << endl;
        
        int diffChoice;
        cin >> diffChoice;
        
        RandomBoardStrategy::Difficulty diff;
        switch(diffChoice) {
            case 1: diff = RandomBoardStrategy::EASY; break;
            case 2: diff = RandomBoardStrategy::MEDIUM; break;
            case 3: diff = RandomBoardStrategy::HARD; break;
            default: diff = RandomBoardStrategy::MEDIUM;
        }
        
        game = SnakeAndLadderGameFactory::createRandomGame(boardSize, diff);
        
    } 
    else if(choice == 3) {
        // Custom game
        int boardSize;
        cout << "Enter board size (e.g., 10 for 10x10 board): ";
        cin >> boardSize;
        
        cout << "Choose custom setup type:" << endl;
        cout << "1. Specify counts only (random placement)" << endl;
        cout << "2. Specify exact positions" << endl;
        
        int customChoice;
        cin >> customChoice;
        
        if(customChoice == 1) {
            int numSnakes, numLadders;
            cout << "Enter number of snakes: ";
            cin >> numSnakes;
            cout << "Enter number of ladders: ";
            cin >> numLadders;
            
            BoardSetupStrategy* strategy = new CustomSetupStrategy(numSnakes, numLadders, true);
            game = SnakeAndLadderGameFactory::createCustomGame(boardSize, strategy);
            delete strategy;
            
        } 
        else {
            int numSnakes, numLadders;
            cout << "Enter number of snakes: ";
            cin >> numSnakes;
            cout << "Enter number of ladders: ";
            cin >> numLadders;
            
            CustomSetupStrategy* strategy = new CustomSetupStrategy(numSnakes, numLadders, false);
            
            // Get snake positions
            for(int i = 0; i < numSnakes; i++) {
                int start, end;
                cout << "Enter snake " << (i+1) << " start and end positions: ";
                cin >> start >> end;
                strategy->addSnakePosition(start, end);
            }
            
            // Get ladder positions
            for(int i = 0; i < numLadders; i++) {
                int start, end;
                cout << "Enter ladder " << (i+1) << " start and end positions: ";
                cin >> start >> end;
                strategy->addLadderPosition(start, end);
            }
            
            game = SnakeAndLadderGameFactory::createCustomGame(boardSize, strategy);
            delete strategy;
        }
    }
    
    if(game == nullptr) {
        cout << "Invalid choice!" << endl;
        return 1;
    }
    
    // Add observer
    IObserver* notifier = new SnakeAndLadderConsoleNotifier();
    game->addObserver(notifier);
    
    // Create players
    int numPlayers;
    cout << "Enter number of players: ";
    cin >> numPlayers;
    
    for(int i = 0; i < numPlayers; i++) {
        string name;
        cout << "Enter name for player " << (i+1) << ": ";
        cin >> name;
        SnakeAndLadderPlayer* player = new SnakeAndLadderPlayer(i+1, name);
        game->addPlayer(player);
    }
    
    // Play the game
    game->play();
    
    // Cleanup
    delete game;
    delete board;
    delete notifier;
    
    return 0;
}