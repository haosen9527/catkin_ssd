#ifndef _MSGBUFFERTYPE_H_
#define _MSGBUFFERTYPE_H_

#include <fastrtps/TopicDataType.h>
#include <opencv2/core/core.hpp>
using namespace eprosima::fastrtps;
using namespace eprosima::fastrtps::rtps;
class MsgBufferType : public eprosima::fastrtps::TopicDataType {
public:
    typedef std::vector<char> type;

	MsgBufferType();
	virtual ~MsgBufferType();
	bool serialize(void *data, SerializedPayload_t *payload);
	bool deserialize(SerializedPayload_t *payload, void *data);
        std::function<uint32_t()> getSerializedSizeProvider(void* data);
	bool getKey(void *data, InstanceHandle_t *ihandle);
	void* createData();
	void deleteData(void * data);
	MD5 m_md5;
	unsigned char* m_keyBuffer;
};

#endif 
