#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <string>

#include "zenoh.hxx"
#include "ExampleIncoming.pb.h"

using namespace std::chrono_literals;

incoming::MessagePack message;

void listener(const zenoh::Sample &sample){
    // float payload = sample.get_payload().deserialize<float>();
    std::string payload = sample.get_payload().as_string();
    message.ParseFromString(payload);
    for (int i = 0; i < message.vehicles_size(); i++){
        const incoming::Vehicle &vehicle = message.vehicles(i);
        std::cout << "ID: " << vehicle.id() << std::endl;
        std::cout << "Throttle: " << vehicle.throttle() << std::endl;
    }

}

int main(){

    zenoh::Config config = zenoh::Config::create_default();
    auto session = zenoh::Session::open(std::move(config));

    auto AcclListen = session.declare_subscriber("carla/in", &listener, zenoh::closures::none);

    while (true){
        std::this_thread::sleep_for(100ms);
    }
}
