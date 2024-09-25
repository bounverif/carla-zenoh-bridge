#include <iostream>
#include <random>
#include <variant>
#include "zenoh.hxx"

using namespace zenoh;

int main(){
    // random number generation setup
    std::random_device rd;

    std::mt19937 e(rd());

    std::uniform_real_distribution<> df(0.0, 1.0);
    std::uniform_int_distribution<> di(1, 6);
    std::uniform_int_distribution<> db(0, 1);

    // test data publish to topics under carla/out
    // i.e. carla/out/throttle
    Config config = Config::create_default();
    auto session = Session::open(std::move(config));

    for (int i = 0; i < 1000; ++i){
        std::cout << "-------------- Instance " << i << " -----------------" << std::endl;
        std::cout << "Throttle: " << df(e) << std::endl;
        std::cout << "Steer: " << df(e) << std::endl;
        std::cout << "Brake: " << df(e) << std::endl;
        std::cout << "Handbrake: " << db(e) << std::endl;
        std::cout << "Reverse: " << db(e) << std::endl;
        std::cout << "Gear: " << di(e) << std::endl;
    }
}