#include "socketForRaspi.hpp"

pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

// make 2 static var for chk the global var(XToken, YToken)
static int Xtmp = 999999;
static int Ytmp = 999999;
  
int main(int argc, char* argv[]){
	// LETs make client side socket communication thread for receiving data
    pthread_t t_id[1];
    pthread_create(t_id, NULL, RX_thread, NULL);
    pthread_detach(t_id[0]);

	ros::init(argc, argv, PUB_NODE_NAME);
	ros::NodeHandle nh; 
	geometry_msgs::Twist cmd_vel; // variable to publish

	// Set publishing TOPIC MSG
	ros::Publisher cmd_vel_pub = nh.advertise<geometry_msgs::Twist>(TOPIC_NAME, 10,true);
	ros::Rate loop_rate(10);

	// just start first
	cmd_vel.linear.x = 0.03;
	cmd_vel.linear.y = 0.0;
	cmd_vel.linear.z = 0.0;
	cmd_vel.angular.x = 0.0;
	cmd_vel.angular.y = 0.0;
	cmd_vel.angular.z = 0.0;
	std::cout << "first published : go" << '\n';

	// Make Timer for control 
	int flag = 0;
	int cnt_1 = 0;
	int cnt_2 = 0;
	int cnt_3 = 0;
    while (ros::ok()){
		// Have Msg be received?
		pthread_mutex_lock(&g_mutex);
		if ( (XToken != 0) && (YToken != 0) ){
			std::cout << "get Token" << '\n';
			Xtmp = XToken; XToken = 0;
			Ytmp = YToken; YToken = 0;
			std::cout << "msg Get! " << Xtmp << " " << Ytmp << '\n';
			flag = 1;
		}
     	pthread_mutex_unlock(&g_mutex); 

		// Go straight!
		if (flag==0){
    		cmd_vel_pub.publish(cmd_vel);    
		}

		// Go back following X pos
		if ( (flag==1) && (cnt_1 < Xtmp) ){
			cmd_vel.linear.x = -0.2;
			cmd_vel.linear.y = 0.0;
			cmd_vel.linear.z = 0.0;
			cmd_vel.angular.x = 0.0;
			cmd_vel.angular.y = 0.0;
			cmd_vel.angular.z = 0.0;
			cmd_vel_pub.publish(cmd_vel);
			std::cout << "go back" << '\n';
			cnt_1++;
		}

		// Rotate bot
		if ((cnt_1 >= Xtmp) && (cnt_2 < 13)){
//		if ( (flag==1) && (cnt_2 < 13)){
			cmd_vel.linear.x = 0.4;
			cmd_vel.linear.y = 0.0;
			cmd_vel.linear.z = 0.0;
			cmd_vel.angular.x = 0.0;
			cmd_vel.angular.y = 0.0;
			cmd_vel.angular.z = 4;
			cmd_vel_pub.publish(cmd_vel);
			std::cout << "rotation " << '\n';
			cnt_2++;
		}

		// Go to Parking lot
		if ( (cnt_2 >= 13) && (cnt_3 < Ytmp)){
			cmd_vel.linear.x = -0.2;
			cmd_vel.linear.y = 0.0;
			cmd_vel.linear.z = 0.0;
			cmd_vel.angular.x = 0.0;
			cmd_vel.angular.y = 0.0;
			cmd_vel.angular.z = 0.0;
			cmd_vel_pub.publish(cmd_vel);
			std::cout << "go parking lot" << '\n';
			cnt_3++;
		}

		// Stop!
		if ( cnt_3 >= Ytmp ){
			cmd_vel.linear.x = 0.0;
			cmd_vel.linear.y = 0.0;
			cmd_vel.linear.z = 0.0;
			cmd_vel.angular.x = 0.0;
			cmd_vel.angular.y = 0.0;
			cmd_vel.angular.z = 0.0;
			cmd_vel_pub.publish(cmd_vel);
			
			flag = 0;
			Xtmp = 0;
			Ytmp = 0;
			std::cout << "end" <<'\n';
			break;
		}

		loop_rate.sleep(); // Keep looping while loop in 0.1s
	} // end of while(ros::ok) loop

	void* exitStatus;
	pthread_cancel(t_id[0]);
	pthread_join(t_id[0], &exitStatus);

    return 0;
}

