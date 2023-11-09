# NSS231012
## Turtlebot3 with ROS noetic

### You have to refer READMD of main branch to install this 

### Job
1. Receive data using socket and thread
2. Hand over data received to main thread(control turtlebot movement)
3. Publish data to OpenCR using cmd_vel_pub.publish()

### Publish
1. cmd_vel type is geometry_msgs/Twist
2. cmd_vel consists of
```
    cmd_vel.linear.x
    cmd_vel.linear.y
    cmd_vel.linear.z
    cmd_vel.angular.x
    cmd_vel.angular.y
    cmd_vel.angular.z
```
cmd_vel.linear y/z are not commonly used. Because They are for y-axis and z-axis in 2D and that is not needed and non-sense.

cmd_vel.angular x/y are not commonly used. The reason is almost same to above.

cmd_vel.linear.x is for moving the bot forward and backward.

cmd_vel.angular.z is for rotate the bot at clock/nonclock wise.
