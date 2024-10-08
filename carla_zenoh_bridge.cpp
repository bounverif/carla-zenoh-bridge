#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>

#include "VehicleSim.hpp"

using namespace zenoh;

/*
    TODO Run this program with mac for full zenoh configurability testing
*/

std::map<std::string, std::string> config;
boost::shared_ptr<cc::Vehicle> vehicle;

int main() {
    // parse the config file
    /*
        TODO Add proper config file support:
            [] .yml or .json
    */
    std::ifstream configFile("config.txt");
    
    if (!configFile.is_open()){
        std::cerr << "Could not open config file" << std::endl;
        return 1;
    }
    
    std::string line;
    while (getline(configFile, line)){
        std::size_t pos = line.find('=');
        config.insert({line.substr(0, pos), line.substr(pos + 1)});
    }

    std::string host = "localhost";
    uint16_t port = 2000u;

    /*
        TODO Add proper connection configuration for CARLA:
        [x] defaults to localhost:2000
        [] port configuration
        [] host configuration
    */
    // if (config.find("carla_port") == config.end()){
    //     std::cout << "No configuration for CARLA found; using default localhost:2000" << std::endl;
    // }
 
    // else{
    //     port = std::stoi(config.find("carla_port"));
    // }

    auto client = cc::Client(host, port);
    client.SetTimeout(40s);

    auto world = client.GetWorld();
    auto temp_vehiclesList = world.GetVehiclesLightStates();
    auto actorId = temp_vehiclesList.begin()->first;
    auto actor = world.GetActor(actorId);
    std::cout << "Connected to actor with ID: " << actor->GetId() << std::endl;
    
    vehicle = boost::static_pointer_cast<cc::Vehicle>(actor);

    // zenoh declarations
    Config config = Config::create_default();
    auto session = Session::open(std::move(config));

    std::string sub_prefix = "carla/" + std::to_string(vehicle->GetId());


    // Vehicle -> CARLA 
    KeyExpr th_expr(sub_prefix + "/throttle");
    KeyExpr st_expr(sub_prefix + "/steer");
    KeyExpr br_expr(sub_prefix + "/brake");
    KeyExpr rv_expr(sub_prefix + "/reverse");
    KeyExpr hb_expr(sub_prefix + "/handbrake");
    KeyExpr gr_expr(sub_prefix + "/gear");
    KeyExpr mg_expr(sub_prefix + "/manual-gear-shift");


    auto ls_th = session.declare_subscriber(th_expr, &listener::l_throttle, closures::none);
    auto ls_st = session.declare_subscriber(st_expr, &listener::l_steer, closures::none);
    auto ls_br = session.declare_subscriber(br_expr, &listener::l_brake, closures::none);
    auto ls_rv = session.declare_subscriber(rv_expr, &listener::l_reverse, closures::none);
    auto ls_hb = session.declare_subscriber(hb_expr, &listener::l_handbrake, closures::none);
    auto ls_gr = session.declare_subscriber(gr_expr, &listener::l_gear, closures::none);
    auto ls_mg = session.declare_subscriber(mg_expr, &listener::l_manual_gear, closures::none);

    // CARLA -> Vehicle
    KeyExpr AcclExpr(sub_prefix + "/acceleration");
    KeyExpr AngVelExpr(sub_prefix + "/angular-velocity");
    KeyExpr TfExpr(sub_prefix + "/transform");
    KeyExpr VelExpr(sub_prefix + "/velocity");

    auto AcclPub = session.declare_publisher(AcclExpr);
    auto AngVelPub = session.declare_publisher(AngVelExpr);
    auto TfPub = session.declare_publisher(TfExpr);
    auto VelPub = session.declare_publisher(VelExpr);




    while (true){
        std::this_thread::sleep_for(100ms);
        AcclPub.put(vehicle->GetAcceleration().Length());
        AngVelPub.put(vehicle->GetAngularVelocity().Length());
        TfPub.put(vehicle->GetTransform().GetForwardVector().Length());
        VelPub.put(vehicle->GetVelocity().Length());
    }


}