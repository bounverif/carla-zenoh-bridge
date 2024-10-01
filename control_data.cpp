#include <iostream>
#include <random>
#include <variant>

#define ZENOHCXX_ZENOHC
#include "zenoh.hxx"

using namespace zenoh;

/*
    FIXME Extreme memory congestion when simulator is shut down
*/

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
    auto p_handbrake = session.declare_publisher(topic_prefix + "/handbrake");
    auto p_reverse = session.declare_publisher(topic_prefix + "/reverse");
    auto p_gear = session.declare_publisher(topic_prefix + "/gear");
    auto p_mgs = session.declare_publisher(topic_prefix + "/manual-gear-shift");

    for (int i = 0; i < 1000000; ++i){
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

        bool handbrake = i < 10000 ? 1 : 0;
        p_handbrake.put(handbrake);
        std::cout << "Handbrake: " << handbrake << std::endl;

        bool reverse = 0;
        p_reverse.put(reverse);
        std::cout << "Reverse: " << reverse << std::endl;

        int gear = di(e);
        p_gear.put(gear);
        std::cout << "Gear: " << gear << std::endl;

        bool mgs = 0;
        p_mgs.put(mgs);
        std::cout << "Manual gear shift: " << mgs << std::endl;
        
        // std::cout << "Handbrake: " << db(e) << std::endl;
        // std::cout << "Reverse: " << db(e) << std::endl;
        // std::cout << "Gear: " << di(e) << std::endl;
    }
}