#!/bin/bash

export NODE_PATH=/usr/lib/node_modules
source /opt/ros/kinetic/setup.bash
source ~/catkin_ws/devel/setup.bash
export ROS_HOSTNAME=localhost
export ROS_MASTER_URI=http://localhost:11311
export PYTHONPATH=/usr/local/lib/python2.7/dist-packages:$PYTHONPATH
source /opt/halcon/.profile_halcon

roscd rovi
roslaunch wrs2018/wrs2018.launch

