#ifndef ZENOHCXX_ZENOHC
#define ZENOHCXX_ZENOHC
#endif

#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <tuple>
#include <vector>

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

class Connection {
    boost::shared_ptr<cc::Vehicle> vehicle;
    zenoh::Session &session;
    std::vector<zenoh::Subscriber<void>> subscribers;
    std::vector<zenoh::Publisher> publishers;

    void bindSubscribers();
    void bindPublishers();

    public:
        Connection();
        Connection(zenoh::Session &session, boost::shared_ptr<cc::Vehicle> vehicle);
        void publish();
};

