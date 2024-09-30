#include "VehicleSim.hpp"

#ifdef DEBUG
#define log(x) std::cout << x << std::endl;
#else
#define log(x)
#endif

// extern cc::Vehicle vehicle;


/*
    This is a temporary constructor, built for quick testing of zenoh networking.
    Change it later with proper vehicle connection handling as well as vehicle select.
    TODO proper vehicle selection functionality
    TODO no existing vehicle throws a segfault

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
        std::cout << "Connected to actor with ID: " << this->actor->GetId() << std::endl;
    }
    catch(...) {
        std::cerr << "No actor found" << std::endl;
    }
}

*/

/*
    Connect to the zenoh session and subscribe to relevant topics.
    TODO Add zenoh configurability & multiple vehicles support
        [] Zenoh session target can be configured
        [] In the case of multiple vehicles, will subscribe to corresponding topics and listen from them


void VehicleSim::connect(){
    
    auto config = zenoh::Config::create_default();
    auto session = zenoh::Session::open(std::move(config));

    std::string sub_prefix = "carla/" + std::to_string(this->actor->GetId());

    auto s_throttle = session.declare_subscriber(sub_prefix + "/throttle", &(this->l_throttle), zenoh::closures::none);
}

*/

// listen to carla/<actor_id>/throttle
void listener::l_throttle(const zenoh::Sample &sample){
    float x = sample.get_payload().deserialize<float>();
    std::cout << "Received " << x << "at " << sample.get_keyexpr().as_string_view() << std::endl;
    cc::Vehicle::Control control;
    control.throttle = x;
    vehicle->ApplyControl(control);
}

void listener::l_steer(const zenoh::Sample &sample){
    float x = sample.get_payload().deserialize<float>();
    std::cout << "Received " << x << "at " << sample.get_keyexpr().as_string_view() << std::endl;
    cc::Vehicle::Control control;
    control.steer = x;
    vehicle->ApplyControl(control);
}

// at carla/<actor_id>/brake
void listener::l_brake(const zenoh::Sample &sample){
    float x = sample.get_payload().deserialize<float>();
    std::cout << "Received " << x << "at " << sample.get_keyexpr().as_string_view() << std::endl;
    cc::Vehicle::Control control;
    control.brake = x;
    vehicle->ApplyControl(control);
}
