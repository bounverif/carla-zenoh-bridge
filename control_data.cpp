#include <iostream>
#include <random>


int main(){
    // random number generation setup
    std::random_device rd;

    std::mt19937 e(rd());

    std::uniform_real_distribution<> df(0.0, 1.0);
    std::uniform_int_distribution<> di(1, 6);
    std::uniform_int_distribution<> db(0, 1);

    // test data publish to stdout
    for (int i = 0; i < 10; ++i){
        std::cout << "-------------- Instance " << i << " -----------------" << std::endl;
        std::cout << "Throttle: " << df(e) << std::endl;
        std::cout << "Steer: " << df(e) << std::endl;
        std::cout << "Brake: " << df(e) << std::endl;
        std::cout << "Handbrake: " << db(e) << std::endl;
        std::cout << "Reverse: " << db(e) << std::endl;
        std::cout << "Gear: " << di(e) << std::endl;
    }
}