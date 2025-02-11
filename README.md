# Carla/Zenoh Bridge
## Description
A two-way bridge software for enabling communication between CARLA Simulator and a Zenoh network. 
## Dependencies
As of now, the bridge has following dependencies:
- Zenoh C++ API: Header-only C++ API bindings for Zenoh. Requires either ZenohC or ZenohPico as a backend.
- LibCarla: CARLA's C++ API as well as its dependencies (boost, libpng, libtiff etc.)
- Protobuf v29.3
## Building
Build process of LibCarla necessarily depends on docker, as the API is built in the environment provided in CARLA documentation. Therefore, before attempting to run the project, one must build at least the ```builder``` stage of Dockerfile.

Before beginning, make sure that ZenohCpp is installed. 

If you want to develop locally, run
```
docker build -t libcarla-container --target builder .
docker create --name temp libcarla-container
docker cp temp:/home/carla/Examples/CppClient/libcarla-install .
```
at the project root to copy LibCarla to the project directory. Note that CMake expects LibCarla at project directory root.

Another option is to directly build the container. The container will then include the latest commit of the master branch.

To build the project using CMake, run 
```
mkdir build && cd build
cmake .
```
at the project root.

## Project Structure
The project currently consists of four parts:
- carla-zenoh-bridge.cpp: This file contains the bridge implementation.
- spawner.cpp: This file is used for spawning actors into the CARLA Simulator.
- control-data.cpp: This file is used for testing and provides a simple control input for the vehicle(s) in the CARLA Simulator environment.
- test-listener.cpp: This file is used for testing and provides a simple listener in the zenoh network.

The bridge connects to existing vehicles and provides message transfer between Zenoh network and CARLA Simulator. Kinematic data is regularly sent from CARLA Simulator to carla/out topic over Zenoh network. Controls from vehicle controllers must be sent to carla/in topic, which the bridge listens and applies any arriving control to the vehicle in the simulator.

As part of the project, ```test-listener.cpp``` listens to carla/out and ```control-data.cpp``` acts as an example controller by sending random control data to carla/in.

## What's Next
- [x] Protobuffers support for transmitting a unified data packet for actors (in place of separate listeners for given basic types). 
- [x] Modify ```control-data.cpp``` and ```test-listener.cpp``` for better actor detection (which may be implemented simultaneously with above). 
- [ ] A full list of dependencies as well as version enforcement may be required for ease of further development.
### Configuration
- [ ] Run-time configuration options for CARLA Simulator address and Zenoh router address. While ```config.txt``` exists and works for now, this project will eventually be implemented in a docker-compose project; thus, environment variables may be a better option.
- [ ] Actor & Map configuration options for ```spawner.cpp```.
- [ ] Run-time configuration options for Zenoh session configuration (such as in/out topic addresses or any necessary configuration options that may arise in the future).

As for a general direction of the future work, one may consider a CLI for the bridge and the spawner. There is also a probable issue to address, which is that the bridge does not act deterministically if two distinct controllers attempt to send a message to the same vehicle ID. Also not all edge cases are covered regarding message serialization & deserialization.

As regards the general architecture of the bridge software, I believe that a bridge software should be minimalistic and agnostic to controllers; that is, firstly, it should only do what it should and should not do what it shouldn't, and secondly, it should provide a unified interface to any controller attempting to access the simulator.
I believe that the current design achieves these goals to some extent and thus, saving any optimization or any missing points from my end, the bridge architecture should not drastically change.


