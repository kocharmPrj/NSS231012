# AutoParking Project
---------------------------------------
The autoparking project aims to facilitate easy parking in complex car parks, reducing time spent parking, and allowing people to utilize their time more effectively.

### Member
- Jinho Kim
- Eunyoung Kim
- Jooeun Park
- Suengkwon Yu
- Jonghyeok Oh

## AVM(Around View Monitor)
Using four cameras, we create an Around View Monitor (AVM) system that displays video on a combined image. The central point of this combined image is the turtle bot, and each camera provides a view of the front, left, right, and rear of the vehicle.

## LaneDetector
By employing Canny edge detection on the parking lot, we establish a Region of Interest (ROI) to inspect whether there are any objects within the parking area.

## Connection Between PC & raspberripi3 on turtlrbot
By socket connect using posix api, a PC and raspi3 make a connetion. Using thread and mutex, LaneDetector part makes decision to move turtlebot to parking lot and socket TX part get that signal on mutex and send the data to raspi. The raspi also runs RX part using thread and mutex to receive the data and utilize it.

## TuttleBot
Using ros1-noetic and TurtleBot3 embedded SW, the turtlebot gets the moving data from raspberry pi3. That depends on Publication in ros communication API.
