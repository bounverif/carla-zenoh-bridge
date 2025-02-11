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

/* 
    Adapted from the official C++ Client example provided in
    CARLA 0.9.15 documentation.
*/

namespace cc = carla::client;
namespace cg = carla::geom;
namespace csd = carla::sensor::data;

using namespace std::chrono_literals;
using namespace std::string_literals;

#define EXPECT_TRUE(pred) if (!(pred)) { throw std::runtime_error(#pred); }

// If number of vehicles is 1, a camera is attached to the vehicle for debugging purposes.
#define NUM_VEHICLES 1

/// Pick a random element from @a range.
template <typename RangeT, typename RNG>
static auto &RandomChoice(const RangeT &range, RNG &&generator) {
  EXPECT_TRUE(range.size() > 0u);
  std::uniform_int_distribution<size_t> dist{0u, range.size() - 1u};
  return range[dist(std::forward<RNG>(generator))];
}

static auto ParseArguments(int argc, const char *argv[]) {
  EXPECT_TRUE((argc == 1u) || (argc == 3u));
  using ResultType = std::tuple<std::string, uint16_t>;
  return argc == 3u ?
      ResultType{argv[1u], std::stoi(argv[2u])} :
      ResultType{"localhost", 2000u};
}

int main(int argc, const char *argv[]) {
  try {

    std::string host;
    uint16_t port;
    std::tie(host, port) = ParseArguments(argc, argv);

    std::mt19937_64 rng((std::random_device())());

    auto client = cc::Client(host, port);
    client.SetTimeout(40s);

    std::cout << "Client API version : " << client.GetClientVersion() << '\n';
    std::cout << "Server API version : " << client.GetServerVersion() << '\n';

    /*
      TODO Add town select as well as random towns.
          [] Town select
          [] Vehicle blueprint select
          [] Spawn point select
        Might be passed as arguments
    */

    // Load a random town.
    auto town_name = RandomChoice(client.GetAvailableMaps(), rng);
    std::cout << "Loading world: " << town_name << std::endl;
    auto world = client.LoadWorld(town_name);

    // Get a random vehicle blueprint.
    auto blueprint_library = world.GetBlueprintLibrary();

    auto vehicles = blueprint_library->Filter("vehicle");
    
    std::vector<cc::BlueprintLibrary::value_type> v_list;
    for (size_t i = 0; i < NUM_VEHICLES; ++i) {
      v_list.push_back(RandomChoice(*vehicles, rng));
    }

    // Randomize the blueprint.
    // if (blueprint.ContainsAttribute("color")) {
    //   auto &attribute = blueprint.GetAttribute("color");
    //   blueprint.SetAttribute(
    //       "color",
    //       RandomChoice(attribute.GetRecommendedValues(), rng));
    // }

    // Find a valid spawn point.
    auto map = world.GetMap();
    std::vector<carla::traffic_manager::ActorPtr> actors;
    for (size_t i = 0; i < NUM_VEHICLES; i++){
      auto transform = RandomChoice(map->GetRecommendedSpawnPoints(), rng);
      actors.push_back(world.SpawnActor(v_list[i], transform));
      std::cout << "Spawned " << actors[i]->GetDisplayId() << '\n';
    }

    /*
      TODO Spawn camera fixed to the actor for debugging purposes.
          May need to be optional for allowing windowless simulator execution.
    */


    carla::traffic_manager::ActorPtr spectator;
    carla::traffic_manager::ActorPtr camera;
    if (NUM_VEHICLES == 1) {
      auto camera_bp = blueprint_library->Find("sensor.other.collision");
      auto c = world.SpawnActor(*camera_bp, cg::Transform{cg::Location{0.0f, 5.0f, 1.0f}, cg::Rotation{0.0f, -90.0f, 0.0f}}, actors[0].get());
      auto s = world.GetSpectator();
      spectator = s;
      camera = c;
    }

    while (true){
      if (NUM_VEHICLES == 1) spectator->SetTransform(camera->GetTransform());
      
      std::this_thread::sleep_for(200ms);
    }

  } catch (const cc::TimeoutException &e) {
    std::cout << '\n' << e.what() << std::endl;
    return 1;
  } catch (const std::exception &e) {
    std::cout << "\nException: " << e.what() << std::endl;
    return 2;
  }
}