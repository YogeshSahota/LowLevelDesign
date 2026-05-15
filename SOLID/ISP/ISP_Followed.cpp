#include <iostream>
#include <stdexcept>

using namespace std;

// Seperate interface for 2D shape
class TwoDimensionalShape {
public:
    virtual double area() = 0;
};

// Seperate interface for 3D shape
class ThreeDimensionalShape {
public:
    virtual double area() = 0;
    virtual double volume() = 0; // 2D shapes don't have volume!
};

// Square is a 2D shape but is forced to implement volume()
class Square : public TwoDimensionalShape {
private:
    double side;

public:
    Square(double s) : side(s) {}

    double area() override {
        return side * side;
    }
};

// Rectangle is also a 2D shape but is forced to implement volume()
class Rectangle : public TwoDimensionalShape {
private:
    double length, width;

public:
    Rectangle(double l, double w) : length(l), width(w) {}

    double area() override {
        return length * width;
    }

};

// Cube is a 3D shape, so it actually has a volume
class Cube : public ThreeDimensionalShape {
private:
    double side;

public:
    Cube(double s) : side(s) {}

    double area() override {
        return 6 * side * side;
    }

    double volume() override {
        return side * side * side;
    }
};

int main() {
    TwoDimensionalShape* square = new Square(5);
    TwoDimensionalShape* rectangle = new Rectangle(4, 6);
    ThreeDimensionalShape* cube = new Cube(3);

    cout << "Square Area: " << square->area() << endl;
    cout << "Rectangle Area: " << rectangle->area() << endl;
    cout << "Cube Area: " << cube->area() << endl;
    cout << "Cube Volume: " << cube->volume() << endl;
    
    return 0;
}