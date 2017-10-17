#include <ros/ros.h>
#include <motion_decoder/image_converter.hpp>
#include <apriltags_ros/AprilTagDetectionArray.h>
#include <apriltags_ros/AprilTagDetection.h>
#include <tf/transform_broadcaster.h>
#include <geometry_msgs/Point.h>
#include <geometry_msgs/Quaternion.h>

using namespace std;
ImageConverter* ic;

//video dimensions
int width = 640;
int height = 480;

//map [-1, 1] to [0, 640]
float mapX(float x){
	return  (x * (height/2)) + height/2 + 80;
}

//map [-1, 1] to [0, 480]
float mapY(float y){
	return  (y * (width/2)) + width/2;
}


void apriltag_detection_callback(const apriltags_ros::AprilTagDetectionArray msg)
{
 
	   //ROS_INFO("Yea boi");
	  //TODO: Parse message and publish transforms as apriltag_tf and camera
	for (int i = 0; i < msg.detections.size(); i++){
	   geometry_msgs::Point point = msg.detections[i].pose.pose.position;
	   geometry_msgs::Quaternion quat = msg.detections[i].pose.pose.orientation;
	   static tf::TransformBroadcaster br;
	   tf::Transform transform;
		
	   transform.setOrigin(tf::Vector3(point.x, point.y, point.z));
	
	   transform.setRotation(tf::Quaternion(quat.x, quat.y, quat.z, quat.w));

	   ic->setTagLocations(mapX(point.x), mapY(point.y), point.z);
	   cout << "[X,Y]: "<< mapX(point.x) << " " << mapY(point.y) << endl;
	   br.sendTransform(tf::StampedTransform(transform, ros::Time::now(), "camera", "april_tf"));

	}
}


int main(int argc, char** argv)
{
  ros::init(argc, argv, "image_converter");
  
  ros::NodeHandle n;
  //TODO: Add a subscriber to get the AprilTag detections The callback function skeleton is given.
  ros::Subscriber sub = n.subscribe("tag_detections", 1000, apriltag_detection_callback);
  ImageConverter converter;
  ic = &converter;
  //ros::spin();
  ros::Rate loop_rate(100);
  ROS_INFO("In main\n");


  while(ros::ok()) {
    ros::spinOnce();
    loop_rate.sleep();
  }
  return 0;
}
