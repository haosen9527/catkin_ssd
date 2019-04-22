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




void RTPS_callback(MsgType msg, ros::Publisher pub)
{   

    std::string label; 
    imagetool::image_sensor rosMsg;
    rosMsg.image_name = msg.label();
    rosMsg.data = deserialize_ros<sensor_msgs::Image>(msg.msg());
    if(pub == NULL)
    {
        std::cout << "pub is NULL" << std::endl;
    }
    // cv_bridge::CvImage(std_msgs::Header(), "bgr8", imgRes).toImageMsg();
    pub.publish(rosMsg);// rosMsg发布
}

int main(int argc, char **argv)
{
    ros::Publisher pub;
    ros::init(argc, argv, "Receiver");
    ros::NodeHandle nh;
    pub = nh.advertise<imagetool::image_sensor>("/ImagequeryResult",10);
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
