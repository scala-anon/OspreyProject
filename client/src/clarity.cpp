#include <ingestion.pb.h>
#include <ingestion.grpc.pb.h>
#include <common.pb.h>

#include <grpc/grpc.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <cstdint>
#include <json.hpp> // Include the nlohmann/json.hpp for JSON parsing since hdr file is close to json format

using namespace dp::service::ingestion;
using namespace google::protobuf::internal;
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
    int providerId = 0001;
    IngestDataRequest_IngestionDataFrame dataFrame = 

}


EventMetadata GetEventMetaData(){
    EventMetadata metaData;
    string description; 
    Timestamp startTime = GetTimeStamp;
    Timestamp stopTime = GetTimeStamp;
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
    string name = "columnExample";
    DataValue dataValue = GetDataValue;
    return dataColumn;
}

DataTimestamps GetDataTimeStamps(){
    DataTimestamps timeStamps;
    SamplingClock samplingClock = GetSamplingClock;
    TimestampList timeStampList = GetTimeStampList;
    return timeStamps;
}

DataValue GetDataValue(){
    DataValue dataValue;
    DataValue_ValueStatus valueStatus = GetValueStatus;
    string stringValue;
    boolean booleanValue; 
    Integer IntValue;
    long longValue;
    float floatValue;
    double doubleValue;
    ByteArray byteArrayValue;
    Array arrayValue = GetArray;
    Structure structureValue = GetStructure;
    Image imageValue = GetImage;
    Timestamp timeStampValue = GetTimeStamp;
    return dataValue;
}

DataValue_ValueStatus GetValueStatus(){
    enum StatusCode {
        NO_STATUS = 0;		// no status condition
      DEVICE_STATUS = 1;	// hardware device condition
      DRIVER_STATUS = 2;	// hardware driver condition
      RECORD_STATUS = 3;	// device database record condition
      DB_STATUS = 4; 		// device database condition
      CONF_STATUS = 5;		// configuration condition
      UNDEFINED_STATUS = 6;	// unknown or undefined status condition
      CLIENT_STATUS = 7;	// data provider client condition
    }
    enum severity {
        NO_ALARM = 0;			// no alarm conditions
      MINOR_ALARM = 1;		// value has triggered a minor alarm condition
      MAJOR_ALARM = 2;		// value has triggered a major alarm condition
      INVALID_ALARM = 3;	// value has triggered an invalid alarm condition
      UNDEFINED_ALARM = 4;	// unknown or undefined alarm condition present
    }
    DataValue_ValueStatus valueStatus;
    string message;
    int statusCode;
    intseverity;
    return valueStatus;
}

Array GetArray(){
    Array array;
    DataValue dataValue = GetDataValue;
    return array;
}

SamplingClock GetSamplingClock(){
    SamplingClock samplingClock;
    Timestamp startTime = GetTimeStamp;
    int periodNano; // needs to be in nanoseconds
    int countField;
    return samplingClock;
}

TimestampList GetTimeStampList(){
    TimestampList stampList;
    Timestamp timeStamp = GetTimeStamp;
    return stampList;
}

Structure GetStructure(){
    Structure structValue;
    Structure_Field fields = GetField;
    return structValue;
}

Image GetImage(){
    Image imageItem;
    enum FileType {
    RAW = 0;
    JPEG = 1;
    GIF = 2;
    TIFF = 3;
    BMP = 4;
    PNG = 5;
    EPS = 6;
    SVG = 7;
    PDF = 8;
  }
    byte image;
    int fileType;
    return imageItem;
}

Structure_Field GetField(){
    Structure_Field field;
    string name;
    DataValue value = GetDataValue;
    return field;
}

TimeStamp GetTimeStamp(){
    Timestamp Time;
    uint64_t epochSec;
    uint64_t nanoSec;
    return time;
}

int main() {
    
}
