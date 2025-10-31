#include "tester.hpp"
#include <cmath>
#include <chrono>
#include <iostream>
#include <thread>
#include <list>

using namespace std;
constexpr long double PI = 3.141592653589793238462643L;
constexpr long double E = 2.718281828459045235360287L;


float wzor(float enkoder,float kat) {
    int mnoznik;

    float roznica = abs(enkoder - kat);
    float speed = 127 * (1 - pow(E,-roznica/20));

    if (enkoder < kat) {
        mnoznik = 1;
    }else {
        mnoznik =  -1;
    }
    return speed * mnoznik;
}

class Points {
private:
    float promien;

public:
    int x,y,z;

    Points(int x, int y, int z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    float katX(){
        return atan((y/x))/ (2*PI) * 4095;
    }

    float katY(){
        promien = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
        return acos(z/promien) / (2*PI) * 4095;
    }
};



int solver(std::shared_ptr<backend_interface::Tester> tester, bool preempt) {

    cout << (preempt ? "Preempt" : "Queue") << '\n';

    uint16_t encoder_value_X = 0;
    uint16_t encoder_value_Y = 0;
    std::list<Points> points;

  auto motor1 = tester->get_motor_1();
  auto motor2 = tester->get_motor_2();
  auto commands = tester->get_commands();

  commands->add_data_callback([&points](const Point& target) {
      points.emplace_back(target.x,target.y,target.z);
      cout << "Cel: x=" << target.x
              << " y=" << target.y
              << " z=" << target.z << endl;

    });

    motor1->add_data_callback([&encoder_value_X](const uint16_t& value){
      encoder_value_X = value;
      cout << "Enkoder 1: " << encoder_value_X << endl;
  });

  motor2->add_data_callback([&encoder_value_Y](const uint16_t& value){
     encoder_value_Y = value;
     cout << "Enkoder 2: " << encoder_value_Y << endl;
 });



    for (const auto& p : points)
        p.print();

    while (true) {
    motor1->send_data(wzor(encoder_value_X,));
    motor2->send_data(wzor(encoder_value_Y,));
    this_thread::sleep_for(chrono::milliseconds(100));
}



  return 0;



}
