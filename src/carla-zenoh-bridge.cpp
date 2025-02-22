#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>

#include "Connection.hpp"

using namespace zenoh;
using namespace std::chrono_literals;

/*
    TODO Run this program with mac for full zenoh configurability testing
*/

std::map<std::string, std::string> config;

int parseConfigFile(const std::string &filePath){
    // parse the config file
    /*
        TODO Add proper config file support:
            [] .yml or .json
    */
    std::ifstream configFile(filePath);
    
    if (!configFile.is_open()){
        std::cerr << "Could not open config file" << std::endl;
        return 1;
    }
    
    std::string line;
    while (getline(configFile, line)){
        std::size_t pos = line.find('=');
        config.insert({line.substr(0, pos), line.substr(pos + 1)});
    }
    return 0;
}

int main() {

    if (parseConfigFile("../config.txt") == 1) return;

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

    /*
        TODO Failsafe behavior on session connection error.
    */
    zenoh::Config config = zenoh::Config::create_default();
    auto session = zenoh::Session::open(std::move(config));

    auto client = cc::Client(host, port);
    client.SetTimeout(40s);

    Context context(session, client);


    auto world = client.GetWorld();

    // set sync mode
    carla::rpc::EpisodeSettings settings;
    settings.synchronous_mode = true;
    world.ApplySettings(settings, 1s);

    auto vehicleLightStatesList = world.GetVehiclesLightStates();
    for (size_t i = 0; i < vehicleLightStatesList.size(); i++){
        auto vehicleActorID = vehicleLightStatesList[i].first;
        std::cout << "Connecting to actor with ID: " << vehicleActorID << std::endl;

        auto vehicleActorPtr = world.GetActor(vehicleActorID);
        boost::shared_ptr<cc::Vehicle> vehicle = boost::static_pointer_cast<cc::Vehicle>(vehicleActorPtr);
        
        context.addVehicleToVehicleList(vehicle);
    }


    while (true){
        std::this_thread::sleep_for(100ms);

        context.publish();

        world.Tick(40s);
    }


}