#include <functional>

#include "Connection.hpp"
#include "listeners.hpp"

/*
    This is a temporary constructor, built for quick testing of zenoh networking.
    Change it later with proper vehicle connection handling as well as vehicle select.
    TODO proper vehicle selection functionality
    TODO no existing vehicle throws a segfault
*/

/*
    TODO Add zenoh configurability & multiple vehicles support
        [] Zenoh session target can be configured
        [] In the case of multiple vehicles, will subscribe to corresponding topics and listen from them
*/

/*
    TODO Add failsafe behavior for subscribers & publishers
        May be redundant if session connection errors are properly handled
*/


Context::Context(zenoh::Session &session) 
: session{session} {
    using namespace std::placeholders;
    auto bin_listener = std::bind(&Context::listen, this, _1);
    auto in_listener = this->session.declare_subscriber("carla/in", bin_listener, zenoh::closures::none);
};

void Context::publish(){
    for (auto &vehicle: this->vehicleList) vehicle.publish();
}

void Context::listen(const zenoh::Sample &sample){

}

Vehicle::Vehicle(Context &context, boost::shared_ptr<cc::Vehicle> vehicle) 
: context(context), vehicle(vehicle) {
        this->bindSubscribers();
        this->bindPublishers();
};


void Vehicle::bindSubscribers(){

    using namespace std::placeholders;

    std::string baseExpr = "carla/" + std::to_string(this->vehicle->GetId());

    auto f_throttle = std::bind(listener::l_throttle, _1, this->vehicle);
    auto f_steer = std::bind(listener::l_steer, _1, this->vehicle);
    auto f_brake = std::bind(listener::l_brake, _1, this->vehicle);
    auto f_gear = std::bind(listener::l_gear, _1, this->vehicle);
    auto f_handbrake = std::bind(listener::l_handbrake, _1, this->vehicle);
    auto f_reverse = std::bind(listener::l_reverse, _1, this->vehicle);
    auto f_manual_gear = std::bind(listener::l_manual_gear, _1, this->vehicle);

    auto ls_th = this->context.session.declare_subscriber(baseExpr + "/throttle", f_throttle, zenoh::closures::none);
    auto ls_st = this->context.session.declare_subscriber(baseExpr + "/steer", f_steer, zenoh::closures::none);
    auto ls_br = this->context.session.declare_subscriber(baseExpr + "/brake", f_brake, zenoh::closures::none);
    auto ls_rv = this->context.session.declare_subscriber(baseExpr + "/reverse", f_reverse, zenoh::closures::none);
    auto ls_hb = this->context.session.declare_subscriber(baseExpr + "/handbrake", f_handbrake, zenoh::closures::none);
    auto ls_gr = this->context.session.declare_subscriber(baseExpr + "/gear", f_gear, zenoh::closures::none);
    auto ls_mg = this->context.session.declare_subscriber(baseExpr + "/manual-gear-shift", f_manual_gear, zenoh::closures::none);

    this->subscribers.push_back(std::move(ls_th));
    this->subscribers.push_back(std::move(ls_st)); 
    this->subscribers.push_back(std::move(ls_br));
    this->subscribers.push_back(std::move(ls_rv));
    this->subscribers.push_back(std::move(ls_hb));
    this->subscribers.push_back(std::move(ls_gr));
    this->subscribers.push_back(std::move(ls_mg));

}

void Vehicle::bindPublishers(){

    std::string baseExpr = "carla/" + std::to_string(this->vehicle->GetId());

    auto AcclPub =      this->context.session.declare_publisher(baseExpr + "/acceleration");
    auto AngVelPub =    this->context.session.declare_publisher(baseExpr + "/angular-velocity");
    auto TfPub =        this->context.session.declare_publisher(baseExpr + "/transform");
    auto VelPub =       this->context.session.declare_publisher(baseExpr + "/velocity");

    this->publishers.push_back(std::move(AcclPub));
    this->publishers.push_back(std::move(AngVelPub));
    this->publishers.push_back(std::move(TfPub));
    this->publishers.push_back(std::move(VelPub));


}

void Vehicle::publish(){

    this->publishers[0].put(this->vehicle->GetAcceleration().Length());
    this->publishers[1].put(this->vehicle->GetAngularVelocity().Length());
    this->publishers[2].put(this->vehicle->GetTransform().location.x);
    this->publishers[3].put(this->vehicle->GetVelocity().Length());

}

void Context::addVehicle(boost::shared_ptr<cc::Vehicle> vehicle){
    this->vehicleList.push_back(std::move(Vehicle(*this, vehicle)));
}