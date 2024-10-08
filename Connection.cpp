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
    Connect to the zenoh session and subscribe to relevant topics.
    TODO Add zenoh configurability & multiple vehicles support
        [] Zenoh session target can be configured
        [] In the case of multiple vehicles, will subscribe to corresponding topics and listen from them
*/

void Connection::bindSubscribers(){

    using namespace std::placeholders;

    std::string baseExpr = "carla/" + std::to_string(this->vehicle->GetId());

    auto f_throttle = std::bind(listener::l_throttle, _1, this->vehicle);
    auto f_steer = std::bind(listener::l_steer, _1, this->vehicle);
    auto f_brake = std::bind(listener::l_brake, _1, this->vehicle);
    auto f_gear = std::bind(listener::l_gear, _1, this->vehicle);
    auto f_handbrake = std::bind(listener::l_handbrake, _1, this->vehicle);
    auto f_reverse = std::bind(listener::l_reverse, _1, this->vehicle);
    auto f_manual_gear = std::bind(listener::l_manual_gear, _1, this->vehicle);

    auto ls_th = this->session.declare_subscriber(baseExpr + "/throttle", f_throttle, zenoh::closures::none);
    auto ls_st = this->session.declare_subscriber(baseExpr + "/steer", f_steer, zenoh::closures::none);
    auto ls_br = this->session.declare_subscriber(baseExpr + "/brake", f_brake, zenoh::closures::none);
    auto ls_rv = this->session.declare_subscriber(baseExpr + "/reverse", f_reverse, zenoh::closures::none);
    auto ls_hb = this->session.declare_subscriber(baseExpr + "/handbrake", f_handbrake, zenoh::closures::none);
    auto ls_gr = this->session.declare_subscriber(baseExpr + "/gear", f_gear, zenoh::closures::none);
    auto ls_mg = this->session.declare_subscriber(baseExpr + "/manual-gear-shift", f_manual_gear, zenoh::closures::none);

    this->subscribers.push_back(std::move(ls_th));
    this->subscribers.push_back(std::move(ls_st)); 
    this->subscribers.push_back(std::move(ls_br));
    this->subscribers.push_back(std::move(ls_rv));
    this->subscribers.push_back(std::move(ls_hb));
    this->subscribers.push_back(std::move(ls_gr));
    this->subscribers.push_back(std::move(ls_mg));

}

void Connection::bindPublishers(){

    std::string baseExpr = "carla/" + std::to_string(this->vehicle->GetId());

    auto AcclPub = this->session.declare_publisher(baseExpr + "/acceleration");
    auto AngVelPub = this->session.declare_publisher(baseExpr + "/angular-velocity");
    auto TfPub = this->session.declare_publisher(baseExpr + "/transform");
    auto VelPub = this->session.declare_publisher(baseExpr + "/velocity");

    this->publishers.push_back(std::move(AcclPub));
    this->publishers.push_back(std::move(AngVelPub));
    this->publishers.push_back(std::move(TfPub));
    this->publishers.push_back(std::move(VelPub));


}

Connection::Connection(zenoh::Session &session, boost::shared_ptr<cc::Vehicle> vehicle) : session{session}{

    this->vehicle = vehicle;
    this->bindSubscribers();
    this->bindPublishers();
}

void Connection::publish(){

    this->publishers[0].put(this->vehicle->GetAcceleration().Length());
    this->publishers[1].put(this->vehicle->GetAngularVelocity().Length());
    this->publishers[2].put(this->vehicle->GetTransform().location.x);
    this->publishers[3].put(this->vehicle->GetVelocity().Length());

}