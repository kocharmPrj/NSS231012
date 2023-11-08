#!/bin/bash

echo "setting the ros.."
process_number=$(ps -aux | grep noetic/bin/rosmaster | grep -v grep | awk '{print $2}')

if [ -n "$process_number" ]; then
	kill -9 $process_number
	echo "killed already running rosmaster ps"
else 
	echo "no ps of rosmaster running.."
fi

sleep 2

source /opt/ros/noetic/setup.bash

sleep 2

roslaunch turtlebot3_bringup turtlebot3_robot.launch &&
(roscore &)

sleep 5

echo "end setting.."
