#ifndef __CMD_VEL_PUB_H__ 
#define __CMD_VEL_PUB_H__ 

#include "geometry_msgs/Twist.h"

struct CmdVelMsg
{
  geometry_msgs::Twist cmd_vel; 
};

#endif  

