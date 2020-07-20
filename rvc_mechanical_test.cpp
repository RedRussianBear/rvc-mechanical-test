// C/C++ Headers
#include <stdlib.h>
#include <string>
#include <algorithm>

// ROS Headers
#include <ros/ros.h>
#include <sensor_msgs/JointState.h>
#include <std_srvs/Empty.h>
#include <std_srvs/Trigger.h>
#include <std_srvs/SetBool.h>

#define READY 1
#define MOVING 2
#define ARRIVED 3

int state = READY;


ros::Publisher pub_move;


// Relative movement subscriber callback
void endofmotion(const sensor_msgs::JointState &msg) {
  state = ARRIVED;
}


int main(int argc, char **argv) {
  // Initialize ROS
  ros::init(argc, argv, "rvc_mechanical_test");
  ros::NodeHandle nh;


  // Define subscribers, publishers, and services
  ros::Rate r(10);

  pub_move = nh.advertise<sensor_msgs::JointState>("move_jp", 1000);
 
  ros::Subscriber goal_sub = nh.subscribe("goal_js", 1000, &endofmotion);

  ros::ServiceClient tap_client = nh.serviceClient<std_srvs::Empty>("tap");
  std_srvs::Empty empt;
  int i = 0;
  int s = 0;
  int x[] = {-50, -250, -250, -50, -150};
  int y[] = {50, 50, 150, 150, 100};
  
  sensor_msgs::JointState msg;

  // Release flow control to ROS
  while(ros::ok() && i < 151) {

    switch(state){
    case READY:

      // Assign message data
      msg.header.seq = s;
      msg.header.stamp = ros::Time::now();
      msg.header.frame_id = "rvc_test";

      msg.name.resize(2);
      msg.name[0] = "x";
      msg.name[1] = "y";

      msg.position.resize(2);
      msg.position[0] = x[i % 5];
      msg.position[1] = y[i % 5];
      s++;

      pub_move.publish(msg);
      state = MOVING;
      
      break;
    case MOVING:
      break;
    case ARRIVED:
      i++;
      tap_client.call(empt);
      state = READY;
      break;

    }
    
    r.sleep();
  }
  
  return 0;
}
