# AutoParking Project
---------------------------------------
The autoparking project aims to facilitate easy parking in complex car parks, reducing time spent parking, and allowing people to utilize their time more effectively.

### Member
- Jinho Kim
- Eunyoung Kim
- Jooeun Park
- Suengkwon Yu
- Jonghyeok Oh

### Install
 - after clone this repo, make some folders for project build in root folder of this project
```
  mkdir build lib
  cd lib
```
 - Install opencv and build it in lib folder by following Instruction below
    - https://docs.opencv.org/4.x/d7/d9f/tutorial_linux_install.html
    - you can get lib/build lib/opencv4.x folder then
<br>
- Let's build and run
```
  cd ../build
  cmake ..
  make 
  cd ../output
  ./NSS231012
```
 
## AVM(Around View Monitor)
Using four cameras, we create an Around View Monitor (AVM) system that displays video on a combined image. The central point of this combined image is the turtle bot, and each camera provides a view of the front, left, right, and rear of the vehicle.

## LaneDetector
By employing Canny edge detection on the parking lot, we establish a Region of Interest (ROI) to inspect whether there are any objects within the parking area.

## Connection Between PC & raspberripi3 on turtlebot
By socket connect using posix api, a PC and raspi3 make a connetion. Using thread and mutex, LaneDetector part makes decision to move turtlebot to parking lot and socket TX part get that signal on mutex and send the data to raspi. The raspi also runs RX part using thread and mutex to receive the data and utilize it.

## TurtleBot
Using ros1-noetic and TurtleBot3 embedded SW, the turtlebot gets the moving data from raspberry pi3. That depends on Publication in ros communication API.
