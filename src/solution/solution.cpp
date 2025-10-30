#include "tester.hpp"
#include <cmath>
#include <chrono>
#include <iostream>
#include <thread>

using namespace std;
constexpr long double PI = 3.141592653589793238462643L;
constexpr long double E = 2.718281828459045235360287L;


float wzor(float enkoder,float kat) {
    int mnoznik;

    float roznica = abs(enkoder - kat);
    float speed = 127 * (1 - pow(E,-roznica/8));

   return speed;
}



int solver(std::shared_ptr<backend_interface::Tester> tester, bool preempt) {

    cout << (preempt ? "Preempt" : "Queue") << '\n';

    Point current_target;
    uint16_t encoder_value = 0;
    uint16_t encoder_value_2 = 0;
    float kat_1;
    float kat_2;
    float promien;

  auto motor1 = tester->get_motor_1();
  auto motor2 = tester->get_motor_2();
  auto commands = tester->get_commands();

  commands->add_data_callback([&current_target,&kat_1,&kat_2,&promien](const Point& target) {
    current_target = target;
    kat_1 = atan((current_target.y/current_target.x))/ (2*PI) * 4095;
    promien = sqrt(pow(current_target.x, 2) + pow(current_target.y, 2) + pow(current_target.z, 2));
    kat_2 = acos(current_target.z/promien) / (2*PI) * 4095;
      cout << "Cel: x=" << target.x
              << " y=" << target.y
              << " z=" << target.z << endl;

      cout << "KAT 1:" << kat_1 << "\n";
      cout << "KAT 2:" << kat_2 << "\n";
      cout << "PROMIEN:" << promien << "\n";
      });
  this_thread::sleep_for(chrono::milliseconds(2000));


  motor1->add_data_callback([&encoder_value](const uint16_t& value){
      encoder_value = value;
      cout << "Enkoder 1: " << encoder_value << endl;
  });

  motor2->add_data_callback([&encoder_value_2](const uint16_t& value){
     encoder_value_2 = value;
     cout << "Enkoder 2: " << encoder_value_2 << endl;
 });


    while (true) {
    motor1->send_data(wzor(encoder_value,kat_1));
    motor2->send_data(wzor(encoder_value_2,kat_2));
    this_thread::sleep_for(chrono::milliseconds(100));
}



  return 0;



}
