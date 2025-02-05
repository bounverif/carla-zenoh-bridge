#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <tuple>
#include <vector>
#include <memory>

#include <carla/client/ActorBlueprint.h>
#include <carla/client/BlueprintLibrary.h>
#include <carla/client/Client.h>
#include <carla/client/Map.h>
#include <carla/client/Sensor.h>
#include <carla/client/TimeoutException.h>
#include <carla/client/World.h>
#include <carla/geom/Transform.h>
#include <carla/image/ImageIO.h>
#include <carla/image/ImageView.h>
#include <carla/sensor/data/Image.h>

namespace cc = carla::client;

#include <zenoh.hxx>
#include "ExampleIncoming.pb.h"
#include "ExampleOutgoing.pb.h"

class Vehicle;

class Context {
   public:
        std::vector<boost::shared_ptr<cc::Vehicle>> vehicleList;
        zenoh::Session &session;
        std::shared_ptr<zenoh::Subscriber<void>> subscriber;
        std::shared_ptr<zenoh::Publisher> publisher;
        cc::Client &client;

        Context(zenoh::Session &session, cc::Client &client);
        void publish();
        void listen(const zenoh::Sample &sample);
        void addVehicle(boost::shared_ptr<cc::Vehicle> vehicle);
    private:
        void applyControls(const incoming::Vehicle &vehicle);
        void setVector(outgoing::Vector3D &vectorMsg, carla::geom::Vector3D vector);
        void setTransform(outgoing::Transform &transformMsg, carla::geom::Transform transform);
        void setRotation(outgoing::Rotation &rotMsg, carla::geom::Rotation rotation);

};

class Vehicle {
    
    Context& context;
    boost::shared_ptr<cc::Vehicle> vehicle;

    std::vector<zenoh::Subscriber<void>> subscribers;
    std::vector<zenoh::Publisher> publishers;

    void bindSubscribers();
    void bindPublishers();

    public:
        Vehicle(Context &context, boost::shared_ptr<cc::Vehicle> vehicle);
        void publish();
};