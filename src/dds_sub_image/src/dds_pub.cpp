#include "DDSPublisher.h"

#include <fastrtps/Domain.h>
#include <fastrtps/utils/eClock.h>
#include <fastrtps/log/Log.h>

#include <iostream>

#include <ros/ros.h>
#include <std_msgs/String.h>
#include <sensor_msgs/image_encodings.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <boost/serialization/map.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/string.hpp> 
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>


#include <unistd.h>

using namespace eprosima;
using namespace fastrtps;
using namespace rtps;


void DDSPublisher::pub_callback(const imagetool::image_sensor::ConstPtr &rosMsg)
{
    sensor_msgs::Image image;
    image.header = rosMsg->data->header;
    image.height = rosMsg->data->height;
    image.width =  rosMsg->data->width;
    image.encoding = rosMsg->data->encoding;
    image.is_bigendian = rosMsg->data->is_bigendian;
    image.step = rosMsg->data->step;
    image.data = rosMsg->data->data;
    std::string label = rosMsg->image_name;
    msg(serialize_ros(image));//ros序列化
    label(serialize_ros(label));
    //std::cout << str1.size() << std::endl;
    //std::vector<char> vec_data(str1.c_str(), str1.c_str()+str1.size());
    /***将通过ros订阅的消息类型转换成Fast-RTPS所能发送的vector<uchar>***/
    /*for(auto e : vec_data)
    {
        RTPS_Publisher.buf.push_back(static_cast<uchar>(e));
    }
    std::cout << "length:::" << RTPS_Publisher.buf.size() << std::endl;*/
    if(!DDS_pub(buf))//发送消息
    eClock::my_sleep(25);
}

/*void pub_callback(const sensor_msgs::Image::ConstPtr& msg)
{
    cv_bridge::CvImagePtr cv_ptr;
    try
    {
        cv_ptr = cv_bridge::toCvCopy(msg,sensor_msgs::image_encodings::RGB8);
    }   
    catch (cv_bridge::Exception& e)
    {
        ROS_ERROR("cv_bridge exception: %s", e.what());
        return;
    }
    std::string encoding;
    encoding = "jpg";
    cv::imencode("."+encoding,cv_ptr->image,RTPS_Publisher.buf);
    std::cout << "length:::" << RTPS_Publisher.buf.size() << std::endl;
    if(!RTPS_Publisher.RTPS_send())
    eClock::my_sleep(25);
}
*/

int main(int argc, char **argv)
{
    DDSPublisher RTPS_Publisher;
    std::cout << "pub started" << std::endl;
    ros::init(argc,argv,"sub_pub_camera_img_new");
    RTPS_Publisher.init("ImageTopic");

    ros::NodeHandle nh;
    //image_transport::ImageTransport it(nh);

    ros::Subscriber sub = nh.subscribe("/image_raw", 3,
     &DDSPublisher::pub_callback, &RTPS_Publisher);//订阅来自摄像头的主题，目的是接受摄像头采集的图片数据

    std::cout << "send succeed" << std::endl;
    ros::spin();
    Domain::stopAll();
    Log::Reset();  
    return 0;
}


