#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#include <carla/geom/Vector3D.h>

#include "zenoh.hxx"

namespace cg = carla::geom;
using namespace std::chrono_literals;

void listener(const zenoh::Sample &sample){
    float payload = sample.get_payload().deserialize<float>();
    std::cout << "Payload: " << payload << std::endl;
}

int main(){
    int ActorId;
    std::cin >> ActorId;

    std::string ChannelPrefix = "carla/" + std::to_string(ActorId);

    zenoh::Config config = zenoh::Config::create_default();
    auto session = zenoh::Session::open(std::move(config));

    auto AcclListen = session.declare_subscriber(ChannelPrefix + "/acceleration", &listener, zenoh::closures::none);

    while (true){
        std::this_thread::sleep_for(100ms);
    }
}
