#include "tester.hpp"
#include <cmath>
#include <chrono>
#include <iostream>
#include <thread>
#include <list>

using namespace std;
constexpr long double PI = 3.141592653589793238462643L;
constexpr long double E = 2.718281828459045235360287L;

double errorPercent = 0.2;


class Points {
public:
    int x,y,z;
    bool done = false;

    Points(int x, int y, int z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    float angleX() const {
        return atan2(y,x)/ (2*PI) * 4095;

    }

    float angleY() const {
        float radius = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
        return acos(z/radius)/ (2*PI) * 4095;
    }

    void markDone(){
        done = true;
    }

};



float speedFormula(float encoder,float angle) {
    int multiplier;

    float difference = abs(encoder - angle);
    float speed = 127 * (1 - pow(E,-difference/20));

    if (encoder < angle) {
        multiplier = 1;
    }else {
        multiplier =  -1;
    }
    return speed * multiplier;
}

void setMotors(Points& p, uint16_t& encoder_value_X, uint16_t& encoder_value_Y,std::shared_ptr<backend_interface::Tester> tester) {
    auto motor1 = tester->get_motor_1();
    auto motor2 = tester->get_motor_2();
    this_thread::sleep_for(chrono::milliseconds(10));
    cout << "Target: " << p.x << " " << p.y << " " << p.z << endl;

    while (abs(p.angleX() - encoder_value_X) > (4095 * errorPercent/ 100) || abs(p.angleY() - encoder_value_Y) > (4095 * errorPercent / 100)) {
        motor1->send_data(speedFormula(encoder_value_X, p.angleX()));
        motor2->send_data(speedFormula(encoder_value_Y, p.angleY()));
    }
    p.markDone();
    cout << "Reached: " << p.x << " " << p.y << " " << p.z << endl;
}





int solver(std::shared_ptr<backend_interface::Tester> tester, bool preempt) {

    cout << (preempt ? "Preempt" : "Queue") << '\n';

    uint16_t encoder_value_X = 0;
    uint16_t encoder_value_Y = 0;
    std::list<Points> points;
    Points current_point = Points(0,0,0);
    bool didPrint = false;

  auto motor1 = tester->get_motor_1();
  auto motor2 = tester->get_motor_2();
  auto commands = tester->get_commands();

  commands->add_data_callback([&points, &current_point, &preempt,&didPrint](const Point& target) {
      points.emplace_back(target.x,target.y,target.z);
      current_point = Points(target.x,target.y,target.z);
      didPrint = false;
      if (preempt) {
           cout << "changing target to: x=" << target.x
                << " y=" << target.y
                << " z=" << target.z << endl;
       } else {
           cout << "added target to queue: x=" << target.x
                << " y=" << target.y
                << " z=" << target.z << endl;
       }
  });

    motor1->add_data_callback([&encoder_value_X](const uint16_t& value){
      encoder_value_X = value;
      // cout << "Encoder X: " << encoder_value_X << endl;
  });

  motor2->add_data_callback([&encoder_value_Y](const uint16_t& value){
     encoder_value_Y = value;
     // cout << "Encoder Y: " << encoder_value_Y << endl;
 });



    if (preempt) {
        while (true){
            if ((abs(current_point.angleX() - encoder_value_X) < (4095 * errorPercent / 100) && abs(current_point.angleY() - encoder_value_Y) < (4095 * errorPercent / 100)) && !didPrint) {
                cout << "Reached: " << current_point.x << " " << current_point.y << " " << current_point.z << endl;
                didPrint = true;
            }
            motor1->send_data(speedFormula(encoder_value_X, current_point.angleX()));
            motor2->send_data(speedFormula(encoder_value_Y, current_point.angleY()));
        }
    }

    if (!preempt) {
        while (true){
            for (auto& p : points) {
                if (!p.done) setMotors(p,encoder_value_X,encoder_value_Y,tester);
            }
        }
    }


    return 0;



}
