#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <tuple>
#include <vector>
#include <memory>
#include <functional>

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

/*
        This class is the main interface responsible from the communication between CARLA
    simulator and Zenoh network. 
        Its aim is such that all attempts at outside communication between devices in the Zenoh
    network and the simulator must go through this class; that is, for any outside actor, communication
    through Zenoh network should be enough and these outside actors must not directly invoke CARLA API.
    Currently, all communications to the simulator must be sent at "carla/in" and all communications from 
    the simulator must be listened at "carla/out".

    Fields:
    - Session: Zenoh session to connect to. 
    - Client: Connection instance to CARLA Client.
        Session and Client must be initialized before invoking the constructor. 
    - Subsciber & Publisher: (Smart) pointers to subscriber and publisher objects.
    - vehicleList: List of vehicles currently connected to.
        This list is not populated within a member function. Currently, every vehicle in the simulation
        is automatically added to this list in carla-zenoh-bridge.cpp, but this may be subject to change.
*/

class Context {
   public:
        zenoh::Session &session;
        std::shared_ptr<zenoh::Subscriber<void>> subscriber;
        std::shared_ptr<zenoh::Publisher> publisher;
        cc::Client &client;
        std::vector<boost::shared_ptr<cc::Vehicle>> vehicleList;

        Context(zenoh::Session &session, cc::Client &client);
        void publish();
        void listenerCallback(const zenoh::Sample &sample);
        void addVehicleToVehicleList(boost::shared_ptr<cc::Vehicle> vehicle);
    private:
        void applyControlsToVehicle(const incoming::Vehicle &vehicle);
        // Converter methods for CARLA API's geometric structures into corresponding message formats.
        void VectorToMsg(outgoing::Vector3D &vectorMsg, carla::geom::Vector3D vector);
        void TransformToMsg(outgoing::Transform &transformMsg, carla::geom::Transform transform);
        void RotationToMsg(outgoing::Rotation &rotMsg, carla::geom::Rotation rotation);
};