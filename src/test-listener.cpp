#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <string>

#include "zenoh.hxx"
#include "ExampleOutgoing.pb.h"

using namespace std::chrono_literals;

outgoing::MessagePack message;

// example function for listening carla/out.
void listener(const zenoh::Sample &sample){
    std::string payload = sample.get_payload().as_string();
    message.ParseFromString(payload);
    for (int i = 0; i < message.vehicles_size(); i++){
        const outgoing::Vehicle &vehicle = message.vehicles(i);
        const outgoing::Vector3D velocity = vehicle.velocity();
        const outgoing::Vector3D position = vehicle.transform().position();
        std::cout << "Vehicle at (" << position.x() << ", " << position.y() << ", " << position.z() << ")" << std::endl;
        std::cout << "Speed of (" << velocity.x() << ", " << velocity.y() << ", " << velocity.z() << ")" << std::endl;
    }

}

int main(){

    zenoh::Config config = zenoh::Config::create_default();
    auto session = zenoh::Session::open(std::move(config));

    auto AcclListen = session.declare_subscriber("carla/out", &listener, zenoh::closures::none);

    while (true){
        std::this_thread::sleep_for(100ms);
    }
}
