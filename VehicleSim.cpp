#include "VehicleSim.hpp"

#ifdef DEBUG
#define log(x) std::cout << x << std::endl;
#else
#define log(x)
#endif

/*
    This is a temporary constructor, built for quick testing of zenoh networking.
    Change it later with proper vehicle connection handling as well as vehicle select.
    TODO proper vehicle selection functionality
*/
VehicleSim::VehicleSim(std::string host, uint16_t port){
    // connect to the simulation.
    auto client = cc::Client(host, port);
    client.SetTimeout(40s);

    try{
        auto world = client.GetWorld();
        auto temp_vehiclesList = world.GetVehiclesLightStates();
        auto actorId = temp_vehiclesList.begin()->first;
        auto actor = world.GetActor(actorId);
        this->actor = actor;
    }
    catch(...) {
        std::cerr << "No actor found" << std::endl;
    }
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
