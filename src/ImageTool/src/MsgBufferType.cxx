#include <fastcdr/FastBuffer.h>
#include <fastcdr/Cdr.h>

#include "MsgBufferType.h"


MsgBufferType::MsgBufferType() {
    setName("OpenCVBuffer");
    m_typeSize = (uint32_t)1024 + 4 /* vector length */ + 4 /*encapsulation*/;
    m_isGetKeyDefined = false;
    m_keyBuffer = nullptr;
}

MsgBufferType::~MsgBufferType() {
    if(m_keyBuffer!=nullptr)
        free(m_keyBuffer);
}

bool MsgBufferType::serialize(void *data, SerializedPayload_t *payload) {
    type *p_type = (type*) data;
    eprosima::fastcdr::FastBuffer fastbuffer((char*) payload->data, payload->max_size); // Object that manages the raw buffer.
    eprosima::fastcdr::Cdr ser(fastbuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN,
            eprosima::fastcdr::Cdr::DDS_CDR);
    payload->encapsulation = ser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;
    // Serialize encapsulation
    ser.serialize_encapsulation();

    try
    {
        ser.serialize(*p_type);
    }
    catch(eprosima::fastcdr::exception::NotEnoughMemoryException& /*exception*/)
    {
        std::cout << "ARGH " << payload->max_size << std::endl;
        return false;
    }

    payload->length = (uint32_t)ser.getSerializedDataLength(); 	//Get the serialized length
    return true;
}

bool MsgBufferType::deserialize(SerializedPayload_t* payload, void* data) {
    type* p_type = (type*) data; 	//Convert DATA to pointer of your type
    eprosima::fastcdr::FastBuffer fastbuffer((char*)payload->data, payload->length); 	// Object that manages the raw buffer.
    eprosima::fastcdr::Cdr deser(fastbuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN,
            eprosima::fastcdr::Cdr::DDS_CDR); // Object that deserializes the data.
    // Deserialize encapsulation.
    deser.read_encapsulation();
    payload->encapsulation = deser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;

    try
    {
        deser.deserialize(*p_type);
    }
    catch(eprosima::fastcdr::exception::NotEnoughMemoryException& /*exception*/)
    {
        return false;
    }

    return true;
}

std::function<uint32_t()> MsgBufferType::getSerializedSizeProvider(void* data) {
    return [data]() -> uint32_t {
        return (uint32_t)static_cast<type*>(data)->size() + 4 /*vector length*/ + 4 /*encapsulation*/;
    };
}

void* MsgBufferType::createData() {
    return (void*)new type();
}

void MsgBufferType::deleteData(void* data) {
    delete((type*)data);
}

bool MsgBufferType::getKey(void *data, InstanceHandle_t* handle) {
    return false;
}
