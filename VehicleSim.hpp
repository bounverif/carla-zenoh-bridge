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
namespace cg = carla::geom;

using namespace std::chrono_literals;

#include <zenoh.hxx>

/*
    FIXME Look for better alternative for this shared variable
            []
*/
extern boost::shared_ptr<cc::Vehicle> vehicle;

namespace listener {
    // System -> CARLA listeners
    void l_throttle(const zenoh::Sample &sample);
    void l_steer(const zenoh::Sample &sample);
    void l_brake(const zenoh::Sample &sample);
    void l_gear(const zenoh::Sample &sample);
    void l_handbrake(const zenoh::Sample &sample);
    void l_reverse(const zenoh::Sample &sample);
    void l_manual_gear(const zenoh::Sample &sample);
}

boost::shared_ptr<cc::Vehicle> getVehicle(cc::World &world);


/*
class VehicleSim {

    boost::shared_ptr<cc::Actor> actor;
    cc::Vehicle::Control control;
    zenoh::Session session;

    public:
        VehicleSim(std::string host, uint16_t port);
        // TODO complete constructor below
        // VehicleSim(std::string host, uint16_t port, carla::ActorId actor_id);

        void connect();
};

*/