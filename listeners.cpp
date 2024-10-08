#include "listeners.hpp"

#ifdef DEBUG
#define log(x) std::cout << x << std::endl;
#else
#define log(x)
#endif


// listen to carla/<actor_id>/throttle
void listener::l_throttle(const zenoh::Sample &sample, boost::shared_ptr<cc::Vehicle> vehicle){
    float x = sample.get_payload().deserialize<float>();
    std::cout << "Received " << x << " at " << sample.get_keyexpr().as_string_view() << std::endl;
    cc::Vehicle::Control control;
    control.throttle = x;
    vehicle->ApplyControl(control);
}

void listener::l_steer(const zenoh::Sample &sample, boost::shared_ptr<cc::Vehicle> vehicle){
    float x = sample.get_payload().deserialize<float>();
    std::cout << "Received " << x << " at " << sample.get_keyexpr().as_string_view() << std::endl;
    cc::Vehicle::Control control;
    control.steer = x;
    vehicle->ApplyControl(control);
}

// at carla/<actor_id>/brake
void listener::l_brake(const zenoh::Sample &sample, boost::shared_ptr<cc::Vehicle> vehicle){
    float x = sample.get_payload().deserialize<float>();
    std::cout << "Received " << x << " at " << sample.get_keyexpr().as_string_view() << std::endl;
    cc::Vehicle::Control control;
    control.brake = x;
    vehicle->ApplyControl(control);
}

void listener::l_gear(const zenoh::Sample &sample, boost::shared_ptr<cc::Vehicle> vehicle){
    int x = sample.get_payload().deserialize<int>();
    std::cout << "Received " << x << " at " << sample.get_keyexpr().as_string_view() << std::endl;
    cc::Vehicle::Control control;
    control.brake = x;
    vehicle->ApplyControl(control);
}

void listener::l_handbrake(const zenoh::Sample &sample, boost::shared_ptr<cc::Vehicle> vehicle){
    bool x = sample.get_payload().deserialize<int>() == 1;
    std::cout << "Received " << x << " at " << sample.get_keyexpr().as_string_view() << std::endl;
    cc::Vehicle::Control control;
    control.hand_brake = x;
    vehicle->ApplyControl(control);

}

void listener::l_reverse(const zenoh::Sample &sample, boost::shared_ptr<cc::Vehicle> vehicle){
    bool x = sample.get_payload().deserialize<int>() == 1;
    std::cout << "Received " << x << " at " << sample.get_keyexpr().as_string_view() << std::endl;
    cc::Vehicle::Control control;
    control.reverse = x;
    vehicle->ApplyControl(control);
}

void listener::l_manual_gear(const zenoh::Sample &sample, boost::shared_ptr<cc::Vehicle> vehicle){
    bool x = sample.get_payload().deserialize<int>() == 1;
    std::cout << "Received " << x << " at " << sample.get_keyexpr().as_string_view() << std::endl;
    cc::Vehicle::Control control;
    control.manual_gear_shift = x;
    vehicle->ApplyControl(control);
}