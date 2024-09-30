#include "VehicleSim.hpp"

#ifdef DEBUG
#define log(x) std::cout << x << std::endl;
#else
#define log(x)
#endif

VehicleSim::VehicleSim(std::string host, uint16_t port, carla::ActorId actor_id){
    // connect to the simulation.
    auto client = cc::Client(host, port);
    client.SetTimeout(40s);

    // connect to vehicle
    auto world = client.GetWorld();
    auto actor = world.GetActor(actor_id);
    this->actor = boost::static_pointer_cast<cc::Actor>(actor);
}


// listen to carla/<actor_id>/throttle
void VehicleSim::l_throttle(const zenoh::Sample &sample){
    float x = sample.get_payload().deserialize<float>();
    this->control.throttle = x;
}

void VehicleSim::l_steer(const zenoh::Sample &sample){
    float x = sample.get_payload().deserialize<float>();
    this->control.steer = x;
}

// at carla/<actor_id>/brake
void VehicleSim::l_brake(const zenoh::Sample &sample){
    float x = sample.get_payload().deserialize<float>();
    this->control.brake = x;
}
