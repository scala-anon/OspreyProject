#include <ingestion.pb.h>
#include <ingestion.grpc.pb.h>
#include <common.pb.h>

#include <google/protobuf/message.h>
#include <google/protobuf/arena.h>
#include <grpc/grpc.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <chrono>

using namespace dp::service::ingestion;
using google::protobuf::Message;
using google::protobuf::Arena;
using namespace std;

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

/*
    DataIngestionRequest
        ├── Attributes (optional list of key-value pairs)
        ├── ClientRequest (string)
        ├── RequestTime
        │   └── Enum of Timestamp
        │       ├── Epoch Seconds
        │       └── Nano Seconds
        ├── EventMetadata
        │   └── Enum
        │       ├── Description (string)
        │       ├── StartTime
        │       │   └── Enum of Timestamp
        │       │       ├── Epoch Seconds
        │       │       └── Nano Seconds
        │       └── StopTime
        │           └── Enum of Timestamp
        │               ├── Epoch Seconds
        │               └── Nano Seconds
        ├── ProviderId (int)
        └── IngestionDataFrame
            ├── DataColumn
            │   └── Strongly Typed Enum
            │       ├── Name (string)
            │       └── DataValue
            │           ├── ValueStatus
            │           │   └── Strongly Typed Enum
            │           │       ├── Message
            │           │       ├── StatusCode
            │           │       └── Severity
            │           ├── StringValue
            │           ├── BooleanValue
            │           ├── IntValue
            │           ├── LongValue
            │           ├── FloatValue
            │           ├── DoubleValue
            │           ├── ByteArrayValue
            │           ├── ArrayValue
            │           │   └── Enum of DataValues
            │           ├── StructureValue
            │           │   └── Enum of Fields
            │           ├── Image
            │           │   └── Enum
            │           │       ├── Image (file name)
            │           │       └── FileType
            │           └── TimestampValue
            │               └── Same Enum as Timestamp
            └── DataTimestamps
                └── Enum
                    ├── SamplingClock
                    │   └── Enum
                    │       ├── StartTime
                    │       │   └── Enum of Timestamp
                    │       │       ├── Epoch Seconds
                    │       │       └── Nano Seconds
                    │       ├── PeriodNano (int)
                    │       └── CountField (int)
                    └── TimestampList
                        └── Enum
                            └── Timestamps
                                ├── Epoch Seconds
                                └── Nano Seconds
*/
IngestDataRequest GetDataRequest(){
    // Attributes optional not needed right now
    string clientRequest = "0001";
    requestTime = GetTimeStamp;
    eventMetaData = GetEventMetaData;
    int providerId = 1;
    IngestDataRequest_IngestionDataFrame dataFrame = GetIngestionDataFrame;

}


EventMetadata GetEventMetaData(){
    EventMetadata metaData;
    string description; 
    startTime = GetTimeStamp;
    stopTime = GetTimeStamp;
    return metaData;
}

IngestDataRequest_IngestionDataFrame GetIngestionDataFrame(){
    IngestDataRequest_IngestionDataFrame dataFrame;
    DataColumn dataColumn = GetDataColumn;
    DataTimestamps dataTimeStamps = GetDataTimeStamps;
    return dataFrame;
}

DataColumn GetDataColumn(){
    DataColumn dataColumn;
    string name = "dataType";
    int columnLen = 1000;
    for(int i = 0; i < 1000; i++){
        DataValue dataValue = GetDataValue();
        dataValue.set_name(name);
        *dataColumn.add_datavalues() = dataValue;
    }
    return dataColumn;
}

DataTimestamps GetDataTimeStamps(){
    DataTimestamps timeStamps;
    SamplingClock samplingClock = GetSamplingClock;
    TimestampList timeStampList = GetTimeStampList;
    return timeStamps;
}

DataValue GetDataValue(){
    // add section that reads from file
    DataValue dataValue;
    DataValue_ValueStatus valueStatus = GetValueStatus;
    // add conditional statement to check if valuestatus is valid 
    /*
    switch(dataValue.type()){
    CASE String:
        string stringValue = data;
    CASE Boolean:
        boolean booleanValue = data; 
    CASE Int:
        Integer IntValue = data;
    CASE Long:
        long longValue = data;
    CASE float:
        float floatValue = data;
    CASE double:
        double doubleValue = data;
    CASE Bytearray:
        ByteArray byteArrayValue = data;
    CASE Array:
        Array arrayValue = GetArray;
    CASE Struct:
        Structure structureValue = GetStructure;
    CASE image:
        Image imageValue = GetImage;
    CASE timestamp:
        timeStampValue = GetTimeStamp;
    }
    */
    return dataValue;
}

DataValue_ValueStatus_StatusCode GetStatusCode(){
    DataValue_ValueStatus_StatusCode statusCode;
    return statusCode
}
DataValue_ValueStatus_Severity GetSeverity(){
    DataValue_ValueStatus_Severity severity;
    return severity;
}
DataValue_ValueStatus GetValueStatus(){
    DataValue_ValueStatus valueStatus;
    string message;
    DataValue_ValueStatus_StatusCode statusCode = GetStatusCode;
    DataValue_ValueStatus_Severity severity = GetSeverity;
    return valueStatus;
}

Array GetArray(){
    Array array;
    DataValue dataValue = GetDataValue;
    return array;
}

SamplingClock GetSamplingClock(){
    SamplingClock samplingClock;
    startTime = GetTimeStamp;
    int periodNano; // needs to be in nanoseconds
    int countField;
    return samplingClock;
}

TimestampList GetTimeStampList(){
    TimestampList stampList;
    timeStamp = GetTimeStamp;
    return stampList;
}

Structure GetStructure(){
    Structure structValue;
    Structure_Field fields = GetField;
    return structValue;
}

Image_FileType GetFileType(){
    Image_FileType fileType;
    return fileType;
}
Image GetImage(){
    Image imageItem;
    byte image;
    Image_FileType fileType = GetFileType;
    return imageItem;
}

Structure_Field GetField(){
    Structure_Field field;
    string name;
    DataValue value = GetDataValue;
    return field;
}

Timestamp GetTimeStamp(){
    Timestamp Time;
    Time.clear_epochseconds();
    Time.clear_nanoseconds();
    uint64_t epochSec = chrono::duration_cast< epochseconds > (chrono::system_clock::now().time_since_epoch());
    uint64_t nanoSec chrono::duration_cast< nanoseconds > (chrono::system_clock::now().time_since_epoch());
    Time.set_epochseconds(epochSec);
    Time.set_nanoseconds(nanoSec);

    return time;
}

int main() {
    
}
