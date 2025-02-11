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
    auto listenerObjectBind = std::bind(&Context::listenerCallback, this, _1);
    auto sub = this->session.declare_subscriber("carla/in", listenerObjectBind, zenoh::closures::none);
    
    // make sure subscriber and publisher are initialized correctly
    static_assert(std::is_same_v<decltype(sub), zenoh::Subscriber<void>>,
                    "Unexpected error at subscriber initialization");
    this->subscriber = std::make_shared<decltype(sub)>(std::move(sub));
    auto pub = this->session.declare_publisher("carla/out");
    static_assert(std::is_same_v<decltype(pub), zenoh::Publisher>,
                    "Unexpected error at publisher initialization");
    this->publisher = std::make_shared<decltype(pub)>(std::move(pub));
};

void Context::VectorToMsg(outgoing::Vector3D &messageTarget, carla::geom::Vector3D vector){
    messageTarget.set_x(vector.x);
    messageTarget.set_y(vector.y);
    messageTarget.set_z(vector.z);
}

void Context::RotationToMsg(outgoing::Rotation &messageTarget, carla::geom::Rotation rotation){
    messageTarget.set_pitch(rotation.pitch);
    messageTarget.set_yaw(rotation.yaw);
    messageTarget.set_roll(rotation.roll);
}

void Context::TransformToMsg(outgoing::Transform &messageTarget, carla::geom::Transform transform){
    this->VectorToMsg(*(messageTarget.mutable_position()), transform.location);
    this->RotationToMsg(*(messageTarget.mutable_rotation()), transform.rotation);
}

void Context::publish(){
    outgoing::MessagePack message;
    for (auto &vehicle: this->vehicleList) {
        outgoing::Vehicle *vehicleMsg = message.add_vehicles();
        vehicleMsg->set_id(vehicle->GetId());
        // TODO: name is useless, remove from message structure
        vehicleMsg->set_name("name");
        this->VectorToMsg(*(vehicleMsg->mutable_acceleration()), vehicle->GetAcceleration());
        this->VectorToMsg(*(vehicleMsg->mutable_angularvelocity()), vehicle->GetAngularVelocity());
        this->TransformToMsg(*(vehicleMsg->mutable_transform()), vehicle->GetTransform());
        this->VectorToMsg(*(vehicleMsg->mutable_velocity()), vehicle->GetVelocity());
    }
    std::string messageAsStr = message.SerializeAsString();
    this->publisher->put(messageAsStr);
}

void Context::applyControlsToVehicle(const incoming::Vehicle &vehicle){
    auto vehicleAsActorPtr = this->client.GetWorld().GetActor(carla::ActorId(vehicle.id()));
    auto vehiclePtr = boost::static_pointer_cast<cc::Vehicle>(vehicleAsActorPtr);

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

// This function is invoked whenever a packet is received from the network.
// Currently, messages are monolithic; thus at each receive, controls are applied
// to all vehicles contained within the message.

void Context::listenerCallback(const zenoh::Sample &sample){
    std::string payload = sample.get_payload().as_string();
    incoming::MessagePack message;
    message.ParseFromString(payload);
    for (int i = 0; i < message.vehicles_size(); i++){
        // apply controls for each vehicle
        const incoming::Vehicle &vehicle = message.vehicles(i);
        this->applyControlsToVehicle(vehicle);
    }

}

void Context::addVehicleToVehicleList(boost::shared_ptr<cc::Vehicle> vehicle){
    this->vehicleList.push_back(std::move(vehicle));
}