#include <iostream>
#include <random>
#include <variant>
#include <thread>
#include <chrono>
#include <stdint.h>
#include <string>

#include "zenoh.hxx"
#include "ExampleIncoming.pb.h"

using namespace zenoh;
using namespace std::chrono_literals;

// random number generation setup
std::random_device rd;

std::mt19937 e(rd());

std::uniform_real_distribution<> df(0.0, 1.0);
std::uniform_int_distribution<> di(1, 6);
std::uniform_int_distribution<> db(0, 1);

/*
    FIXME Extreme memory congestion when simulator is shut down
*/

std::vector<int> actorsList;
std::string messageAsStr;

void generateVehicleData(incoming::Vehicle* vehicle, int actorId){
    vehicle->set_id(actorId);
    vehicle->set_name("test " + std::to_string(actorId));
    vehicle->set_throttle(1);
    vehicle->set_brake(0);
    vehicle->set_steer(df(e));
    vehicle->set_gear(1);
    vehicle->set_handbrake(0);
    vehicle->set_reverse(0);
    vehicle->set_manual_gear_shift(0);
}

void generateMessage(incoming::MessagePack* message){
    for (auto actor : actorsList){
        generateVehicleData(message->add_vehicles(), actor);
    }
}

// implemented directly from zenohcpp example
void publishMessage(zenoh::Publisher &publisher, incoming::MessagePack *message){
    // these two lines convert the message into a byte-element array
    messageAsStr = message->SerializeAsString();
    publisher.put(messageAsStr);
}

int main(){

    // test data publish to /carla/in
    Config config = Config::create_default();
    auto session = Session::open(std::move(config));

    // save current actors to a list
    while (1) {
        int actorId;
        std::cin >> actorId;
        if (actorId < 0) break;
        actorsList.push_back(actorId);
    }


    auto ctl_publisher = session.declare_publisher("carla/in");

    for (int i = 0; i < 100000; ++i){
        std::this_thread::sleep_for(1s);
        incoming::MessagePack message;
        generateMessage(&message);
        publishMessage(ctl_publisher, &message);
        std::cout << "Instance : " << i << std::endl;
        /*
        std::cout << "-------------- Instance " << i << " -----------------" << std::endl;
        float throttle = df(e);
        p_throttle.put(throttle);
        std::cout << "Throttle: " << throttle << std::endl;

        float steer = df(e);
        p_steer.put(steer);
        std::cout << "Steer: " << steer << std::endl;

        float brake = df(e);
        p_brake.put(brake);
        std::cout << "Brake: " << brake << std::endl;

        int handbrake = i < 10000 ? 1 : 0;
        p_handbrake.put(handbrake);
        std::cout << "Handbrake: " << handbrake << std::endl;

        int reverse = 0;
        p_reverse.put(reverse);
        std::cout << "Reverse: " << reverse << std::endl;

        int gear = di(e);
        p_gear.put(gear);
        std::cout << "Gear: " << gear << std::endl;

        int mgs = 0;
        p_mgs.put(mgs);
        std::cout << "Manual gear shift: " << mgs << std::endl;
        
        // std::cout << "Handbrake: " << db(e) << std::endl;
        // std::cout << "Reverse: " << db(e) << std::endl;
        // std::cout << "Gear: " << di(e) << std::endl;

        */
    }
}