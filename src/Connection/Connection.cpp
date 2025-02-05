#include <functional>

#include "Connection.hpp"

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


Context::Context(zenoh::Session &session, cc::Client &client) 
: session{session}, client{client} {
    using namespace std::placeholders;
    auto bin_listener = std::bind(&Context::listen, this, _1);
    auto sub = this->session.declare_subscriber("carla/in", bin_listener, zenoh::closures::none);
    static_assert(std::is_same_v<decltype(sub), zenoh::Subscriber<void>>,
                    "Unexpected error at subscriber initialization");
    this->subscriber = std::make_shared<decltype(sub)>(std::move(sub));
    auto pub = this->session.declare_publisher("carla/out");
    static_assert(std::is_same_v<decltype(pub), zenoh::Publisher>,
                    "Unexpected error at publisher initialization");
    this->publisher = std::make_shared<decltype(pub)>(std::move(pub));
};

void Context::setVector(outgoing::Vector3D &vectorMsg, carla::geom::Vector3D vector){
    vectorMsg.set_x(vector.x);
    vectorMsg.set_y(vector.y);
    vectorMsg.set_z(vector.z);
}

void Context::setRotation(outgoing::Rotation &rotMsg, carla::geom::Rotation rotation){
    rotMsg.set_pitch(rotation.pitch);
    rotMsg.set_yaw(rotation.yaw);
    rotMsg.set_roll(rotation.roll);
}

void Context::setTransform(outgoing::Transform &transformMsg, carla::geom::Transform transform){
    this->setVector(*(transformMsg.mutable_position()), transform.location);
    this->setRotation(*(transformMsg.mutable_rotation()), transform.rotation);
}

void Context::publish(){
    outgoing::MessagePack message;
    for (auto &vehicle: this->vehicleList) {
        outgoing::Vehicle *vehicleMsg = message.add_vehicles();
        vehicleMsg->set_id(vehicle->GetId());
        vehicleMsg->set_name("name");
        this->setVector(*(vehicleMsg->mutable_acceleration()), vehicle->GetAcceleration());
        this->setVector(*(vehicleMsg->mutable_angularvelocity()), vehicle->GetAngularVelocity());
        this->setTransform(*(vehicleMsg->mutable_transform()), vehicle->GetTransform());
        this->setVector(*(vehicleMsg->mutable_velocity()), vehicle->GetVelocity());
    }
    std::string messageAsStr = message.SerializeAsString();
    this->publisher->put(messageAsStr);
}

void Context::applyControls(const incoming::Vehicle &vehicle){
    auto vehicleActorPtr = this->client.GetWorld().GetActor(carla::ActorId(vehicle.id()));
    auto vehiclePtr = boost::static_pointer_cast<cc::Vehicle>(vehicleActorPtr);

    cc::Vehicle::Control control;
    control.throttle = vehicle.throttle();
    control.steer = vehicle.steer();
    control.brake = vehicle.brake();
    control.gear = vehicle.gear();
    control.hand_brake = vehicle.handbrake();
    control.reverse = vehicle.reverse();
    control.manual_gear_shift = vehicle.manual_gear_shift();
    vehiclePtr->ApplyControl(control);


}

void Context::listen(const zenoh::Sample &sample){
    std::string payload = sample.get_payload().as_string();
    incoming::MessagePack message;
    message.ParseFromString(payload);
    for (int i = 0; i < message.vehicles_size(); i++){
        // apply controls for each vehicle
        const incoming::Vehicle &vehicle = message.vehicles(i);
        this->applyControls(vehicle);
    }

}



void Context::addVehicle(boost::shared_ptr<cc::Vehicle> vehicle){
    this->vehicleList.push_back(std::move(vehicle));
}