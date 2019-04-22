#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/highgui/highgui.hpp>
#include "test/image_sensor.h"

using namespace std;
using namespace cv;

void chatterCallback(const test::image_sensor::ConstPtr &msg)
{
    //cout<<msg->image_name<<endl;
   // sensor_msgs::ImagePtr rosimg = boost::shared_ptr<sensor_msgs::Image>();
    string jpg_name=msg->image_name; //"."+msg->image_name.substr(msg->image_name.find_last_of("/"),msg->image_name.find_last_of("."));
    cout<<"save ./images/* succeed"<<jpg_name<<endl;

    string save_image_path = "./images/"+jpg_name;

    sensor_msgs::ImagePtr rosimg(new sensor_msgs::Image(msg->data));
    //save
    imwrite(save_image_path,cv_bridge::toCvShare(rosimg, "bgr8")->image);
}


int main(int argc, char **argv)
{
        ros::init(argc, argv, "sub_image");
        ros::NodeHandle n;

        ros::Subscriber sub = n.subscribe("accuracy_low_image", 1, chatterCallback);

        ros::spin();

        return 0;
}
