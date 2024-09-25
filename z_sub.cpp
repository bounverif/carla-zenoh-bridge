#include "zenoh.hxx"
#include <iostream>
using namespace zenoh;

int main(int argc, char **argv) {
   Config config;
   auto session = expect<Session>(open(std::move(config)));
   auto subscriber = expect<Subscriber>(
      session.declare_subscriber("demo/example/simple", [](const Sample& sample) {
         std::cout << "Received: " << sample.get_payload().as_string_view() << std::endl;
      })
   );
   // Wait for a key press to exit
   char c = getchar();
}