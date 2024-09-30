#include <iostream>
#include <random>
#include <variant>

#define ZENOHCXX_ZENOHC
#include "zenoh.hxx"

using namespace zenoh;

int main(){
    // random number generation setup
    std::random_device rd;

    std::mt19937 e(rd());

    std::uniform_real_distribution<> df(0.0, 1.0);
    std::uniform_int_distribution<> di(1, 6);
    std::uniform_int_distribution<> db(0, 1);

    // test data publish to topics under carla/<actor_id>
    // i.e. carla/<actor_id>/throttle
    Config config = Config::create_default();
    auto session = Session::open(std::move(config));

    int actorId;
    std::cin >> actorId;

    std::string topic_prefix = "carla/" + std::to_string(actorId);

    auto p_throttle = session.declare_publisher(topic_prefix + "/throttle");
    auto p_steer = session.declare_publisher(topic_prefix + "/steer");
    auto p_brake = session.declare_publisher(topic_prefix + "/brake");

    for (int i = 0; i < 100000000; ++i){
        std::cout << "-------------- Instance " << i << " -----------------" << std::endl;
        float throttle = df(e);
        p_throttle.put(throttle);
        std::cout << "Throttle: " << throttle << std::endl;

        float steer = df(e);
        p_steer.put(steer);
        std::cout << "Steer: " << steer << std::endl;

        float brake = df(e);
        p_brake.put(brake);
        std::cout << "Brake: " << brake << std::endl;
        
        // std::cout << "Handbrake: " << db(e) << std::endl;
        // std::cout << "Reverse: " << db(e) << std::endl;
        // std::cout << "Gear: " << di(e) << std::endl;
    }
}