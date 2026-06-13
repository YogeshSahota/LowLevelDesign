#include <iostream>
#include <vector>
#include <queue>

using namespace std;

class IObserver{
    public:
        virtual void update(const string &msg) = 0;
};

class ConsoleNotifier: public IObserver{
    public:
        void update(const string &msg){
            cout  << "[Notification]: " << msg << endl;
        }
};

class Symbol{
    private:
        char mark;
    public: 
        Symbol(char m){
            mark=m;
        }

        char getMark(){
            return this->mark;
        }
};

class Board{
    private:   
        vector<vector<Symbol*>> grid;
        int size;
        Symbol* emptyCell;
    public:
        Board(int size){
            this->size = size;
            this->emptyCell = new Symbol('_');
            grid = vector(size,vector(size,emptyCell));
        }
        bool isCellEmpty(int row, int col){
            if(row<0||row>=size||col<0||col>=size) return false;

            return grid[row][col] == emptyCell;
        }
        bool placeMark(int row, int col,Symbol* s){
            if(!isCellEmpty(row,col)) return false;

            grid[row][col] = s;

            return true;
        }
        Symbol* getCell(int row, int col){
            if(row<0||row>=size||col<0||col>=size) return emptyCell;

            return grid[row][col];
        }
        int getSize(){
            return this->size;
        }
        Symbol* getEmptyCell(){
            return this->emptyCell;
        }
        void display(){
            cout << "\n  ";
        for(int i = 0; i < size; i++) {
            cout << i << " ";
        }
        cout << endl;
        
        for(int i = 0; i < size; i++) {
            cout << i << " ";
                for(int j = 0; j < size; j++) {
                    cout << grid[i][j]->getMark() << " ";
                }
                cout << endl;
            }
            cout << endl;
        }
};

class Rules{
    public:
        virtual bool checkValid(Board* board, int row, int col) = 0;
        virtual bool checkWin(Board* board, Symbol* s) = 0;
        virtual bool checkDraw(Board* board) = 0;
};

class StandardRules: public Rules{
    public:
        bool checkValid(Board* board, int row, int col){
            if(board->isCellEmpty(row,col)) return true;
            return false;
        }
        bool checkWin(Board* board, Symbol* s){
            int n = board->getSize();

            for(int i=0;i<n;i++){
                bool win = true;
                for(int j=0;j<n;j++){
                    if(board->getCell(i,j)!=s){
                        win = false;
                        break;
                    }
                }
                if(win) return true;
            }

            for(int i=0;i<n;i++){
                bool win = true;
                for(int j=0;j<n;j++){
                    if(board->getCell(j,i)!=s){
                        win = false;
                        break;
                    }
                }
                if(win) return true;
            }

            bool win = true;
            for(int i=0;i<n;i++){
                if(board->getCell(i,i)!=s){
                    win = false;
                    break;
                }
            }
            if(win) return true;

            win = true;

            for(int i=0;i<n;i++){
                if(board->getCell(i,n-1-i)!=s){
                    win = false;
                    break;
                }
            }
            if(win) return true;

            return false;
        }

        bool checkDraw(Board* board){
            int n = board->getSize();
            for(int i=0;i<n;i++){
                for(int j=0;j<n;j++){
                    if(board->isCellEmpty(i,j)) return false;
                }
            }
            return true;
        }
};

class Player{
    private:
        int playerId;
        string name;
        Symbol* symbol;
        int score;

    public:
        Player(int playerId, string name, Symbol* symbol){
            this->playerId=playerId;
            this->name=name;
            this->symbol=symbol;
            this->score=0;
        }

        int getPlayerId(){
            return this->playerId;
        }
        string getName(){
            return this->name;
        }
        Symbol* getSymbol(){
            return this->symbol;
        }
        int getScore(){
            return this->score;
        }
        void incrementScore() { 
            score++;
        }
};

enum class GameType{
    STANDARD
};

class TicTacToeGame{
    private:
        Board* board;
        queue<Player*> players;
        vector<IObserver*> observers;
        Rules* rules;
        bool gameOver;
    public:
        TicTacToeGame(int size, Rules* rules){
            this->board = new Board(size);
            this->rules = rules;
            gameOver = false;
        }

        void addPlayer(Player* player){
            players.push(player);
        }
        void addObserver(IObserver* observer){
            observers.push_back(observer);
        }
        void notify(const string msg){
            for(auto observer:observers){
                observer->update(msg);
            }
        }

        void play(){
            if(players.size()<2){
                cout << "Need atleast 2 players to start the game" <<endl;
            }

            notify("TicTacToe game started!");
            
            while(!gameOver){
                Player* currPlayer = players.front();
                board->display();
                cout << currPlayer->getName() << " (" << currPlayer->getSymbol()->getMark() << ") - Enter row and column: ";
                int row,col;
                cin>>row>>col;
                if(rules->checkValid(board,row,col)){
                    board->placeMark(row,col,currPlayer->getSymbol());
                    notify(currPlayer->getName() + " played (" + to_string(row) + "," + to_string(col) + ")");
                    if(rules->checkWin(board,currPlayer->getSymbol())){
                        board->display();
                        notify(currPlayer->getName()+ " wins!");
                        currPlayer->incrementScore();
                        gameOver=true;
                    }else if(rules->checkDraw(board)){
                        notify("Game is Draw!");
                        gameOver=true;
                    }else{
                        players.pop();
                        players.push(currPlayer);
                    }
                }else{
                    cout << "Please enter a valid row & col." <<endl;
                }
            }
        }
        ~TicTacToeGame() {
            delete board;
            delete rules;
        }
};

class GameFactory{
    public:
        static TicTacToeGame* createGame(GameType type, int boardSize){
            if(type==GameType::STANDARD){
                Rules* rules = new StandardRules();
                return new TicTacToeGame(boardSize,rules);
            }
            cout<< "Invalid Game Type!" <<endl;
            return nullptr;
        }
};

int main(){
    cout << "=== TIC TAC TOE GAME ===" << endl;
    
    // Create game with custom board size
    int boardSize;
    cout << "Enter board size (e.g., 3 for 3x3): ";
    cin >> boardSize;
    
    TicTacToeGame* game = GameFactory::createGame(GameType::STANDARD, boardSize);
    
    // Add observer
    IObserver* notifier = new ConsoleNotifier();
    game->addObserver(notifier);
    
    // Create players with custom symbols
    Player* player1 = new Player(1, "Yogesh", new Symbol('X'));
    Player* player2 = new Player(2, "Harshita", new Symbol('O'));
    
    game->addPlayer(player1);
    game->addPlayer(player2);
    
    // Play the game
    game->play();
    
    // Cleanup
    delete game;
    delete player1;
    delete player2;
    delete notifier;
    
    return 0;
}