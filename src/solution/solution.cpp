#include "tester.hpp"
#include <cmath>
#include <chrono>
#include <iostream>
#include <thread>

using namespace std;
int solver(std::shared_ptr<backend_interface::Tester> tester, bool preempt) {

  constexpr long double PI = 3.141592653589793238462643L;
  Point current_target;
  uint16_t encoder_value = 0;
  uint16_t encoder_value_2 = 0;

  //std::cout << (preempt ? "Preempt" : "Queue") << '\n';

  auto motor1 = tester->get_motor_1();
  auto motor2 = tester->get_motor_2();
  auto commands = tester->get_commands();

  commands->add_data_callback([&current_target](const Point& target) {
    current_target = target;
    std::cout << "Cel: x=" << target.x
              << " y=" << target.y
              << " z=" << target.z << std::endl;
  });
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));


  motor1->add_data_callback([&encoder_value](const uint16_t& value){
      encoder_value = value;
      std::cout << "Pozycja enkodera: " << encoder_value << std::endl;
  });

  motor2->add_data_callback([&encoder_value_2](const uint16_t& value){
     encoder_value_2 = value;
     std::cout << "Pozycja enkodera 2: " << encoder_value_2 << std::endl;
 });


  commands->add_data_callback([](const Point& point) {
    std::cout << "Command point: (" << point.x << ", " << point.y << ", " << point.z << ")\n";
  });




  std::cout << "X:" << current_target.x << "\n";
  std::cout << "Y:" << current_target.y << "\n";
  std::cout << "Z:" << current_target.z << "\n";

  float kat_1 = atan((current_target.x/current_target.y))/ (2*PI) * 4095;
  float promien = sqrt(pow(current_target.x, 2) + pow(current_target.y, 2) + pow(current_target.z, 2));
  float kat_2 = acos(current_target.z/promien) / (2*PI) * 4095;

  std::cout << "KAT 1:" << kat_1 << "\n";
  std::cout << "KAT 2:" << kat_2 << "\n";
  std::cout << "PROMIEN:" << promien << "\n";


while (encoder_value < kat_1) {
  motor1->send_data(100);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

  while (encoder_value_2 < kat_2) {
    motor2->send_data(100);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }




  std::this_thread::sleep_for(std::chrono::milliseconds(400));



  return 0;



}
