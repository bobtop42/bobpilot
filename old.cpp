#include <iostream>
using namespace std;

enum { ROWS = 10, CLMS = 10 };
enum class Box { o, X, P };

Box grid[ROWS][CLMS];

const int MAX_NUM = 10;
const int RS_MAX_NUM = 20;

const int routeXY[MAX_NUM][2] = {
    {4, 1}, {4, 2}, {5, 3}, {5, 4}, {4, 4},
    {4, 5}, {3, 5}, {2, 6}, {2, 7}, {1, 8},
};

struct Point {
    int x_ = 0, y_ = 0;

    Point& operator+=(const Point& other) {
        x_ += other.x_;
        y_ += other.y_;
        return *this;
    }
};

struct Compass {
    static constexpr Point N_ = {0, -1};
    static constexpr Point E_ = {1, 0};
    static constexpr Point S_ = {0, 1};
    static constexpr Point W_ = {-1, 0};
};

struct Plane {
    Point loc_ = {2, 2};
    Compass direction_;

    void move(const Point& dir) {
        loc_ += dir;
    }
};

void popGrid(Box grid[ROWS][CLMS], const Plane& plane) {
    bool errorPopGrid = false;
    
    for (int i = 0; i < ROWS; ++i) {
      for (int j = 0; j < CLMS; ++j) {
          bool isMarked = false;
          bool planeMarked = false;
          
          grid[i][j] = Box::o;
          
          for(int k=0; k<MAX_NUM; ++k){
            if (i == routeXY[k][0] && j == routeXY[k][1]) {
                grid[i][j] = Box::X;
                isMarked = true;
                break;
            }
          }
              
          if (i == plane.loc_.x_ && j == plane.loc_.y_) {
              grid[i][j] = Box::P;
              planeMarked = true;
              break;
          } 
              //else if (!isMarked) {
              //grid[i][j] = Box::o;
              //break;
      }
  }

          // if (false) {
          //     errorPopGrid = true;
          //     cout << "popGrid error at iteration:\n";
          //     break;
          // }
}
    


void printGrid(const Box grid[ROWS][CLMS]) {
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < CLMS; ++j) {
            switch (grid[i][j]) {
                case Box::o: cout << "o "; break;
                case Box::X: cout << "X "; break;
                case Box::P: cout << "P "; break;
            }
        }
        cout << endl;
    }
    cout << "----------" << endl;
}

void nextMove(const int routeXY[MAX_NUM][2], Plane& plane, int& rS) {
    int nextX = routeXY[rS][0] - plane.loc_.x_;
    int nextY = routeXY[rS][1] - plane.loc_.y_;

    bool errorNextMove = false;
    bool errorRouteStartTooHigh = false;

    int N=0, E=0, S=0, W=0;

    while (nextX != 0 || nextY != 0) {
        if (nextY < 0) {
            plane.move(Compass::N_);
            N++;
            return;
        } 
        else if (nextX < 0) {
            plane.move(Compass::W_);
            E++;
            return;
        } 
        else if (nextY > 0) {
            plane.move(Compass::S_);
            S++;
            return;
        } 
        else if (nextX > 0) {
            plane.move(Compass::E_);
            W++;
            return;
        } 
        else {
            errorNextMove = true;
            cout << "Next Move Error.\nXY moves left: (" << nextX << "," << nextY << ").\n";
            return;
        }
        
    }

}

int main(){
    Plane plane;
    int rS = 0;

    for (int step = 0; step < MAX_NUM; ++step) {
        popGrid(grid, plane);
        cout << "\nStep " << step + 1 << ":\n";
        printGrid(grid);

        ++rS;

        if (rS >= RS_MAX_NUM) {
            cout << "Route ended.\n";
            break;
        }

        nextMove(routeXY, plane, rS);
    }

    cout << "\nFinal destination reached:\n";
    cout << "Plane XY: (" << plane.loc_.x_ << ", " << plane.loc_.y_ << ")\n";
    return 0;
}
