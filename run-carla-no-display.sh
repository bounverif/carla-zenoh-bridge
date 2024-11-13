#!/bin/sh

sudo docker run --privileged --gpus all --net=host -v /tmp/.X11-unix:/tmp/.X11-unix:rw carlasim/carla:0.9.15 /bin/bash ./CarlaUE4.sh -RenderOffScreen
