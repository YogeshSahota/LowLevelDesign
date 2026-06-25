#include <iostream>
#include <vector>
using namespace std;

enum class Color {
    WHITE,
    BLACK
};

class Position {
public:
    int row;
    int col;

    Position(int r, int c) : row(r), col(c) {}
};

class Piece {
protected:
    Color color;

public:
    Piece(Color c) : color(c) {}

    virtual bool isValidMove(Position from, Position to) = 0;
    virtual string getName() = 0;

    Color getColor() { return color; }

    virtual ~Piece() {}
};

class Pawn : public Piece {
public:
    Pawn(Color c) : Piece(c) {}

    bool isValidMove(Position from, Position to) override {
        int direction = (color == Color::WHITE) ? -1 : 1;
        return to.row == from.row + direction && to.col == from.col;
    }

    string getName() override {
        return "Pawn";
    }
};

class Rook : public Piece {
public:
    Rook(Color c) : Piece(c) {}

    bool isValidMove(Position from, Position to) override {
        return from.row == to.row || from.col == to.col;
    }

    string getName() override {
        return "Rook";
    }
};

class Knight : public Piece {
public:
    Knight(Color c) : Piece(c) {}

    bool isValidMove(Position from, Position to) override {
        int dr = abs(to.row - from.row);
        int dc = abs(to.col - from.col);
        return (dr == 2 && dc == 1) || (dr == 1 && dc == 2);
    }

    string getName() override {
        return "Knight";
    }
};

class Board {
private:
    vector<vector<Piece*>> grid;

public:
    Board() {
        grid.resize(8, vector<Piece*>(8, nullptr));
    }

    void placePiece(Piece* piece, Position pos) {
        grid[pos.row][pos.col] = piece;
    }

    Piece* getPiece(Position pos) {
        return grid[pos.row][pos.col];
    }

    bool movePiece(Position from, Position to) {
        Piece* piece = getPiece(from);

        if (!piece) return false;

        if (!piece->isValidMove(from, to))
            return false;

        grid[to.row][to.col] = piece;
        grid[from.row][from.col] = nullptr;

        return true;
    }
};

class Player {
private:
    string name;
    Color color;

public:
    Player(string name, Color color)
        : name(name), color(color) {}

    Color getColor() { return color; }
    string getName() { return name; }
};

class Game {
private:
    Board board;
    Player white;
    Player black;
    Color currentTurn;

public:
    Game()
        : white("Player1", Color::WHITE),
          black("Player2", Color::BLACK) {
        currentTurn = Color::WHITE;
    }

    Board& getBoard() {
        return board;
    }

    bool makeMove(Position from, Position to) {
        Piece* piece = board.getPiece(from);

        if (!piece) return false;

        if (piece->getColor() != currentTurn)
            return false;

        bool moved = board.movePiece(from, to);

        if (moved) {
            currentTurn =
                currentTurn == Color::WHITE
                ? Color::BLACK
                : Color::WHITE;
        }

        return moved;
    }
};

int main() {
    Game game;

    Board& board = game.getBoard();

    board.placePiece(new Pawn(Color::WHITE), Position(6,0));
    board.placePiece(new Rook(Color::BLACK), Position(0,0));

    bool success =
        game.makeMove(Position(6,0), Position(5,0));

    cout << "Move Success: " << success << endl;

    return 0;
}
