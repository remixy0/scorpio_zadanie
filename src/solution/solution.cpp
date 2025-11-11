#include "tester.hpp"
#include <cmath>
#include <chrono>
#include <iostream>
#include <thread>
#include <list>

using namespace std;
constexpr long double PI = 3.141592653589793238462643L;
constexpr long double E = 2.718281828459045235360287L;
double errorPercent = 0.5;

class Points {
public:
    int x,y,z,directionX,directionY;
    bool done = false;


    Points(int x, int y, int z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    float angleX() const {
        float angle = (atan2(y,x)/ (2*PI) * 4095) >= 0 ? (atan2(y,x)/ (2*PI) * 4095) : 4095 + (atan2(y,x)/ (2*PI) * 4095) ;
        return angle;
    }

    float angleY() const {
        if (z==0) return 0;
        float radius = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
        float angle = (acos(z/radius) / PI * 2048 + 1024);
        if (angle > 1024 && angle < 2048) {
            angle = 1024 - abs(angle - 1024);
        }
        if (angle > 2048 && angle < 3072) {
            angle = 3072 + abs(angle - 3072);
        }
        return angle;
    }

    void markDone(){
        done = true;
    }

    void setDirectionX(int direction) {
        this->directionX = direction;
    }

    void setDirectionY(int direction) {
        this->directionY = direction;
    }
};

int16_t difference(float encoder, float angle) {
    int16_t difference = angle - encoder;
    difference = abs((difference + 2048) % 4096 - 2048);

    if (encoder < angle) {
        if (difference > 2048) difference = -difference;
    }
    else if (difference < 2048) difference = -difference;

    return difference;
}

int8_t speedFormula(float encoder, float angle) {
    int16_t diff = difference(encoder, angle);
    int8_t speed = 127 * (1 - pow(E,-abs(diff)/20));
    return static_cast<int8_t>(speed);
}


void setMotors(Points& p, uint16_t& encoder_value_X, uint16_t& encoder_value_Y,std::shared_ptr<backend_interface::Tester> tester) {
    auto motor1 = tester->get_motor_1();
    auto motor2 = tester->get_motor_2();
    this_thread::sleep_for(chrono::milliseconds(10));
    cout << "Target: " << p.x << " " << p.y << " " << p.z << endl;
    cout << "Angles: " << p.angleX() << " " << p.angleY() << endl;

    p.setDirectionX((difference(encoder_value_X,p.angleX()) > 0) ? 1 : -1);
    p.setDirectionY((difference(encoder_value_Y,p.angleY()) > 0) ? 1 : -1);

    while (abs(difference(encoder_value_X,p.angleX())) > (4095 * errorPercent/ 100) || abs(difference(encoder_value_Y,p.angleY())) > (4095 * errorPercent / 100)) {
        motor1->send_data(speedFormula(encoder_value_X, p.angleX())*p.directionX);
        motor2->send_data(speedFormula(encoder_value_Y, p.angleY())*p.directionY);
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

    commands->add_data_callback([&points, &current_point, &preempt,&didPrint, &encoder_value_X, &encoder_value_Y](const Point& target) {
        points.emplace_back(target.x,target.y,target.z);
        current_point = Points(target.x,target.y,target.z);
        current_point.setDirectionX((difference(encoder_value_X,current_point.angleX()) > 0) ? 1 : -1);
        current_point.setDirectionY((difference(encoder_value_Y,current_point.angleY()) > 0) ? 1 : -1);
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

        while (preempt){
            if ((abs(difference(encoder_value_X,current_point.angleX())) < (4095 * errorPercent / 100) && abs(difference(encoder_value_Y,current_point.angleY())) < (4095 * errorPercent / 100)) && !didPrint) {
                cout << "Reached: " << current_point.x << " " << current_point.y << " " << current_point.z << endl;
                didPrint = true;
            }
            motor1->send_data(speedFormula(encoder_value_X, current_point.angleX())*current_point.directionX);
            motor2->send_data(speedFormula(encoder_value_Y, current_point.angleY())*current_point.directionY);
        }

        while (!preempt){
            for (auto& p : points) {
                if (!p.done) {
                    setMotors(p,encoder_value_X,encoder_value_Y,tester);

                };
        }}


    return 0;
}
