#include "DDSPublisher.h"

#include <fastrtps/participant/Participant.h>
#include <fastrtps/attributes/ParticipantAttributes.h>
#include <fastrtps/attributes/PublisherAttributes.h>
#include <fastrtps/publisher/Publisher.h>
#include <fastrtps/Domain.h>
#include <fastrtps/utils/eClock.h>
#include <fastrtps/TopicDataType.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace eprosima::fastrtps;
using namespace eprosima::fastrtps::rtps;

DDSPublisher::DDSPublisher():im_participant(nullptr),im_publisher(nullptr)
{

}

bool DDSPublisher::init(std::string m_topic)
{
    ParticipantAttributes PParam;
    PParam.rtps.defaultSendPort = 11511;
    PParam.rtps.use_IP6_to_send = true;
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

    PublisherAttributes Wparam;
    Wparam.topic.topicKind = NO_KEY;
    Wparam.topic.topicDataType = m_type.getName();
    Wparam.topic.topicName = m_topic;
    Wparam.topic.historyQos.kind = KEEP_LAST_HISTORY_QOS;
    Wparam.topic.historyQos.depth = 30;
    Wparam.topic.resourceLimitsQos.max_samples = 50;
    Wparam.topic.resourceLimitsQos.allocated_samples = 20;
    Wparam.times.heartbeatPeriod.seconds = 2;
    Wparam.times.heartbeatPeriod.fraction = 200*1000*1000;
    Wparam.qos.m_reliability.kind = RELIABLE_RELIABILITY_QOS;
    Wparam.qos.m_publishMode.kind = ASYNCHRONOUS_PUBLISH_MODE;
    Wparam.historyMemoryPolicy = PREALLOCATED_WITH_REALLOC_MEMORY_MODE;
    im_publisher = Domain::createPublisher(im_participant,Wparam,(PublisherListener*)&m_listener);
    if(im_publisher == nullptr)
        return false;

    return true;
}

DDSPublisher::~DDSPublisher()
{
    Domain::removeParticipant(im_participant);
}

void DDSPublisher::PubListener::onPublicationMatched(Publisher* /*pub*/,MatchingInfo& info)
{
    if(info.status == MATCHED_MATCHING)
    {
        n_matched++;
        std::cout << "Publisher matched" <<std::endl;
    }
    else
    {
        n_matched--;
        std::cout << "Publisher unmatched" <<std::endl;
    }
}

bool DDSPublisher::DDS_pub(std::vector<char> buf)
{
    if(m_listener.n_matched > 0)
    {
        im_publisher->write((void*)&buf);
        return true;
    }
    return false;
}

