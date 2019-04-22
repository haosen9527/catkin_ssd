#include "DDSSubscriber.h"

#include <fastrtps/participant/Participant.h>
#include <fastrtps/attributes/ParticipantAttributes.h>
#include <fastrtps/attributes/SubscriberAttributes.h>
#include <fastrtps/subscriber/Subscriber.h>
#include <fastrtps/Domain.h>
#include <fastrtps/utils/eClock.h>
#include <fastrtps/TopicDataType.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace eprosima::fastrtps;
using namespace eprosima::fastrtps::rtps;

DDSSubscriber::DDSSubscriber():im_participant(nullptr),im_subscriber(nullptr)
{

}

bool DDSSubscriber::init(std::string m_topic)
{
    _member_CB = NULL;
    ParticipantAttributes PParam;
    PParam.rtps.defaultSendPort = 10043;
    PParam.rtps.builtin.use_SIMPLE_RTPSParticipantDiscoveryProtocol = true;
    PParam.rtps.builtin.use_SIMPLE_EndpointDiscoveryProtocol = true;
    PParam.rtps.builtin.m_simpleEDP.use_PublicationReaderANDSubscriptionWriter = true;
    PParam.rtps.builtin.m_simpleEDP.use_PublicationWriterANDSubscriptionReader = true;
    PParam.rtps.builtin.domainId = 80;
    PParam.rtps.builtin.leaseDuration = c_TimeInfinite;
    PParam.rtps.setName("Participant");
    im_participant = Domain::createParticipant(PParam);
    if(im_participant == nullptr)
        return false;
    
    Domain::registerType(im_participant,&m_type);
    SubscriberAttributes Rparam;
    Rparam.topic.topicKind = NO_KEY;
    Rparam.topic.topicDataType = m_type.getName();
    Rparam.topic.topicName = m_topic;
    Rparam.topic.historyQos.kind = KEEP_LAST_HISTORY_QOS;
    Rparam.topic.historyQos.depth = 30;
    Rparam.topic.resourceLimitsQos.max_samples = 50;
    Rparam.topic.resourceLimitsQos.allocated_samples = 20;
    Rparam.qos.m_reliability.kind = RELIABLE_RELIABILITY_QOS;
    Rparam.historyMemoryPolicy = PREALLOCATED_WITH_REALLOC_MEMORY_MODE;
    
    im_subscriber = Domain::createSubscriber(im_participant,Rparam,(SubscriberListener*)&m_listener);

    if(im_subscriber == nullptr)
        return false;

    return true;
}

DDSSubscriber::~DDSSubscriber() 
{
    Domain::removeParticipant(im_participant);
}

void DDSSubscriber::SubListener::onSubscriptionMatched(Subscriber* /*sub*/,MatchingInfo& info)
{
    if(info.status == MATCHED_MATCHING)
    {
        n_matched++;
        std::cout << "Subscriber matched" << std::endl;
    }
    else
    {
        n_matched--;
        std::cout << "Subscriber unmatched" << std::endl;
    }
}

bool DDSSubscriber::DDS_recv(Subscriber* sub)
{
    if(sub->takeNextData((void*)&m_listener.m_data, &m_listener.m_info))
    {
        struct timeval tv;
        gettimeofday(&tv,NULL);
        Time_t tmp;
        tmp.seconds = (int32_t)tv.tv_sec;
        tmp.fraction = (uint32_t)tv.tv_usec*10000;
        Time_t t2;
        t2.seconds = tmp.seconds - m_listener.m_info.sourceTimestamp.seconds;
        t2.fraction = tmp.fraction - m_listener.m_info.sourceTimestamp.fraction;
        std::cout << "time is" << tmp.seconds << " . " << tmp.fraction << std::endl;
        std::cout << "time stamp" << m_listener.m_info.sourceTimestamp.seconds << "." << m_listener.m_info.sourceTimestamp.fraction << std::endl;
        std::cout << "delay is" << (double)t2.seconds + (double)t2.fraction/100000000000 << std::endl;
        if(m_listener.m_info.sampleKind == ALIVE)
            return true;
        else
            return false;
        
        }
    else
        return false;
}

void DDSSubscriber::DDS_sub(callback CB, ros::Publisher pub)
{
    if(m_listener.n_matched > 0)
    {
        _member_CB = CB;
        if(DDS_recv(im_subscriber))
        _member_CB(m_listener.m_data, pub);
    }
    
}
