#include "DDSSubscriber.h"

#include <fastrtps/Domain.h>
#include <fastrtps/utils/eClock.h>
#include <fastrtps/log/Log.h>

#include <ros/ros.h>
#include <cv_bridge/cv_bridge.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <unistd.h>

using namespace eprosima;
using namespace fastrtps;
using namespace rtps;




void RTPS_callback(std::vector<char> msg, ros::Publisher pub)
{   
    imagetool::image_sensor image;
    std::string str_temp;
    str_temp = std::string(reinterpret_cast<char*>(&msg[0]),msg.size());
    image = deserialize_ros<imagetool::image_sensor>(str_temp);
    std::cout << "length:::" << str_temp.size() << std::endl;

    if(pub == NULL)
    {
        std::cout << "pub is NULL" << std::endl;
    }
    // cv_bridge::CvImage(std_msgs::Header(), "bgr8", imgRes).toImageMsg();
    pub.publish(image);//ros：：Publisher将 image发布
}

int main(int argc, char **argv)
{
    ros::Publisher pub;
    ros::init(argc, argv, "rtps_sub");
    ros::NodeHandle nh;
    pub = nh.advertise<imagetool::image_sensor>("sub",10);
    std::cout << "sub started" <<std::endl;

    DDSSubscriber RTPS_Subscriber;
        if(RTPS_Subscriber.init("ImageTopic"))
        {
            while(ros::ok())
            {
                
                RTPS_Subscriber.DDS_sub(&RTPS_callback, pub);//回调函数
                
            }
        }

    Domain::stopAll();
    Log::Reset();
    return 0;
}
