#include "tester.hpp"

// You can remove or add any includes you need
#include <chrono>
#include <iostream>
#include <thread>

int solver(std::shared_ptr<backend_interface::Tester> tester, bool preempt) {



  // Short example you can remove it
  std::cout << (preempt ? "Preempt" : "Queue") << '\n';
  auto motor1 = tester->get_motor_1();
  auto motor2 = tester->get_motor_2();
  auto commands = tester->get_commands();
  motor1->add_data_callback([](const uint16_t& data) {
    std::cout << "Motor 1 data: " << static_cast<int>(data) << "\n";
  });
  motor2->add_data_callback([](const uint16_t& data) {
    std::cout << "Motor 2 data: " << static_cast<int>(data) << "\n";
  });
  commands->add_data_callback([](const Point& point) {
    std::cout << "Command point: (" << point.x << ", " << point.y << ", " << point.z << ")\n";
  });
  motor1->send_data(100);
  motor2->send_data(-50);
  std::this_thread::sleep_for(std::chrono::milliseconds(250));
  //
  return 0;



}
