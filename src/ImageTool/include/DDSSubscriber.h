#ifndef IMAGESUBSCRIBER_H_
#define IMAGESUBSCRIBER_H_

#include "MsgBufferType.h"

#include <fastrtps/fastrtps_fwd.h>
#include <fastrtps/attributes/SubscriberAttributes.h>
#include <fastrtps/subscriber/SubscriberListener.h>
#include <fastrtps/subscriber/SampleInfo.h>

#include <ros/ros.h>
#include <cv_bridge/cv_bridge.h>


#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include "imagetool/image_sensor.h"

using namespace eprosima::fastrtps;
using namespace eprosima::fastrtps::rtps;
typedef boost::function<void(std::vector<char>, ros::Publisher)> callback;

class DDSSubscriber {
public:
    DDSSubscriber();
    virtual ~DDSSubscriber();
    bool init(std::string m_topic);
    bool DDS_recv(Subscriber* sub);
    void DDS_sub(callback CB, ros::Publisher pub);
    Subscriber* im_subscriber;

private:
    Participant* im_participant;
    callback _member_CB;
public:
    class SubListener:public SubscriberListener
    {
    public:
        SubListener():n_matched(0){};
        ~SubListener(){};
        void onSubscriptionMatched(Subscriber* sub,MatchingInfo& info);
        std::vector<char> m_data;
        SampleInfo_t m_info;
        int n_matched;
    }m_listener;
private:
    MsgBufferType m_type;

};

template<class T>
    T deserialize_ros(std::string str)
    {
        T t;
        uint32_t serial_size = str.length();
        std::vector<uint8_t> buffer(serial_size);
        std::copy(str.begin(), str.begin() + serial_size, buffer.begin());
        ros::serialization::IStream istream(buffer.data(), serial_size);
        ros::serialization::Serializer<T>::read(istream, t);
        return t;
    }

#endif
