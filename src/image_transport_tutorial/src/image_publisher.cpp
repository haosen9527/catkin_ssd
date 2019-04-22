#include <ros/ros.h>
#include <image_transport/image_transport.h>
//#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/dnn.hpp>
#include <cv_bridge/cv_bridge.h>
#include "test/image_sensor.h"
#include <iostream>
#include <dirent.h>
#include <vector>

using namespace std;




int main(int argc, char** argv)
{
  ros::init(argc, argv, "image_publisher");
  ros::NodeHandle nh;
  ros::NodeHandle private_nh("~"); // 私有空间命名
  std::string pub_topic = "/image_raw";
  float pub_rate = 0.5;
  private_nh.param("pubTopic", pub_topic, pub_topic);
  private_nh.param("pubRate",  pub_rate,  pub_rate);

  test::image_sensor msgs;

  //  image_transport::ImageTransport it(nh);
  //  image_transport::Publisher pub = it.advertise(pub_topic, 1);//ImagequeryResult
  ros::Publisher pub = nh.advertise<test::image_sensor>("/recogImage", 10);

  image_transport::ImageTransport it(nh);
  image_transport::Publisher pub2 = it.advertise("pub_image", 10);

  ros::Rate loop_rate(pub_rate);

  struct dirent *ptr;
  DIR *dir;
  dir = opendir(argv[1]);
  while ((ptr=readdir(dir))!=NULL)
  {
    if(ptr->d_name[0] == '.')
      continue;
    string dir_name = argv[1];
    string file_name = ptr->d_name;

    string full_path = dir_name+file_name;
    msgs.image_name = ptr->d_name;
    std::cout << "send image: " << full_path<<std::endl;

    cv::Mat image = cv::imread(full_path, CV_LOAD_IMAGE_COLOR);
    sensor_msgs::ImagePtr msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", image).toImageMsg();

    sensor_msgs::Image msg_data ;
    cv_bridge::CvImage(std_msgs::Header(), "bgr8", cv_bridge::toCvShare(msg, "bgr8")->image).toImageMsg(msg_data);
    msgs.data= msg_data;

    pub.publish(msgs);
    pub2.publish(msg_data);
    ros::spinOnce();
    loop_rate.sleep();
  }
}
