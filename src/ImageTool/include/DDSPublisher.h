#ifndef DDSPUBLISHER_H_
#define DDSPUBLISHER_H_

#include "MsgBufferType.h"

#include <fastrtps/fastrtps_fwd.h>
#include <fastrtps/attributes/PublisherAttributes.h>
#include <fastrtps/publisher/PublisherListener.h>

#include <ros/ros.h>
#include <std_msgs/String.h>
#include <sensor_msgs/image_encodings.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>

#include "imagetool/image_sensor.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace eprosima::fastrtps;

class DDSPublisher {
public:
    DDSPublisher();
    virtual ~DDSPublisher();
    bool init(std::string m_topic);
    bool DDS_pub(std::vector<char> buf);
    //std::vector<char> buf;
    void pub_callback(const imagetool::image_sensor::ConstPtr& msg);

/*private:*/
    Participant* im_participant;
    Publisher* im_publisher;
    class PubListener:public PublisherListener
    {
    public:
        PubListener():n_matched(0){};
        ~PubListener(){};
        void onPublicationMatched(Publisher* pub,MatchingInfo& info);
        int n_matched;

    }m_listener;
    MsgBufferType m_type;

};

template<class T>
    std::string serialize_ros(T t)
    {
        std::string str;
        uint32_t serial_size = ros::serialization::serializationLength(t);
        boost::shared_array<uint8_t> buffer(new uint8_t[serial_size]);
        ros::serialization::OStream stream(buffer.get(), serial_size);
        ros::serialization::serialize(stream, t);
        std::copy(buffer.get(), buffer.get() + serial_size, std::back_inserter(str));
        return str;
    }

#endif