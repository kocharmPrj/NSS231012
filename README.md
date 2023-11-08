# AutoParking Project
---------------------------------------
![](https://github.com/kocharmPrj/NSS231012/blob/gif/car.gif)
The autoparking project aims to facilitate easy parking in complex car parks, reducing time spent parking, and allowing people to utilize their time more effectively.

### Member
- Jinho Kim
- Eunyoung Kim
- Jooeun Park
- Suengkwon Yu
- Jonghyeok Oh

### Install
##### ServerSide(PC)
 - After clone this repo's main branch, make some folders for project build at root folder of this project
```
  mkdir build lib
  cd lib
```
 - Install opencv and build it in lib folder by following Instruction below
    - https://docs.opencv.org/4.x/d7/d9f/tutorial_linux_install.html
    - you will get lib/build lib/opencv4.x folder then
 - Let's build and run
```
  cd ../build
  cmake ..
  make 
  cd ../output
  ./NSS231012
  ```
 - If there is an err with socket like connect(), check port Number and firewall. The socket in code uses port number 5000. So access to 5000 should be allowed. Or you can change the port number to use in socket_self.cpp
```
  sudo ufw status
```
##### ClientSide(Turtlebot)
 - Install ROS noetic to SBC(raspberry)
    - http://wiki.ros.org/noetic/Installation/Ubuntu
    - Ubuntu 20.04 LTS
 - Install OpenCR 
    - https://emanual.robotis.com/docs/en/platform/turtlebot3/opencr_setup/#opencr-setup
    - On SBC Ubuntu and install firmware to OpenCR
 - Make workspace for ros at home directory and clone clientSide branch of this repo in it.
  ```
    mkdir catkin_ws && cd &_
    git clone -b clientSide <repo url>
  ```
  - Then, you will see a src folder like
```
    src/
    ├── CMakeLists.txt -> /opt/ros/noetic/share/catkin/cmake/toplevel.cmake
    └── raspi
        ├── CMakeLists.txt
        ├── include
        │   └── cmd_vel_pub
        │       └── Cmd_vel_msg.hpp
        ├── msg
          │   └── Cmd_vel_msg.msgd
        ├── package.xml
        └── src
            ├── hdr
            │   └── socketForRaspi.hpp
            ├── raspi.cpp
            └── socket_pi.cpp
```
  - You should amend IP and Port number at src/raspi/src/socket_pi.cpp whether they are right to serverSide(main) 
    - Fill in IP by amending CP and port is 5000 at default
  - Let's build and run
    - Source ros script
    ```source /opt/ros/noetic/setup.bash```
    - There will be a 'setting_ros.bash' to auto settings for ros master and connect between SBC and OpenCR. If there is a problem, refer to Instruction below
    ```./setting_ros.bash```
    - https://emanual.robotis.com/docs/en/platform/turtlebot3/bringup/#bringup
    - And open another terminal of SBC,
    ```
    source /opt/ros/noetic/setup.bash
    cd ~/catkin_ws
    catkin_make
    source devel/setup.bash
    ./run.sh
    ```
    - If there is a problem about package, try 'rosdep' cmd


 
## AVM(Around View Monitor)
Using four cameras, we create an Around View Monitor (AVM) system that displays video on a combined image. The central point of this combined image is the turtle bot, and each camera provides a view of the front, left, right, and rear of the vehicle.

## LaneDetector
By employing Canny edge detection on the parking lot, we establish a Region of Interest (ROI) to inspect whether there are any objects within the parking area.

## Connection Between PC & raspberripi3 on turtlebot
By socket connect using posix api, a PC and raspi3 make a connetion. Using thread and mutex, LaneDetector part makes decision to move turtlebot to parking lot and socket TX part get that signal on mutex and send the data to raspi. The raspi also runs RX part using thread and mutex to receive the data and utilize it.

## TurtleBot
Using ros1-noetic and TurtleBot3 embedded SW, the turtlebot gets the moving data from raspberry pi3. That depends on Publication in ros communication API.
<br><br><br><br><br>



###### This project is primarily licensed under the Apache License 2.0. The 3-Clause BSD License is included as a secondary license for compatibility.
