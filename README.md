# Carla/Zenoh Bridge
## Description
A two-way bridge software for enabling communication between CARLA Simulator and a Zenoh network. 
## Dependencies
As of now, the bridge has following dependencies:
- Zenoh C++ API: Header-only C++ API bindings for Zenoh. Requires either ZenohC or ZenohPico as a backend.
- LibCarla: CARLA's C++ API as well as its dependencies (boost, libpng, libtiff etc.)
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

For now, most of the project has a very crude entrypoint: The project expects all four files to be running at once. ```spawner``` will output the IDs of spawned vehicles, of which one of them must be inputted to ```control-data``` and ```test-listener```. This behavior is only temporary and is subject to change. 
## What's Next
- [ ] Run-time configuration options for CARLA Simulator address and Zenoh router address. While ```config.txt``` exists and works for now, this project will eventually be implemented in a docker-compose project; thus, environment variables may be a better option.
- [ ] Protobuffers support for transmitting a unified data packet for actors (in place of separate listeners for given basic types). 
- [ ] Modify ```control-data.cpp``` and ```test-listener.cpp``` for better actor detection (which may be implemented simultaneously with above). 

