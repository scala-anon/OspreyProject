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

/*Data Request
    ├── Timestamp
    │   └── Strongly Typed Enum
    │       ├── Epoch Seconds
    │       └── Nano Seconds
    │
    └── Data Column
        └── Strongly Typed Enum
            ├── Name (string)
            └── DataValue
                ├── ValueStatus
                │   └── Strongly Typed Enum
                │       ├── Message
                │       ├── StatusCode
                │       └── Severity
                ├── StringValue
                ├── BooleanValue
                ├── IntValue
                ├── LongValue
                ├── FloatValue
                ├── DoubleValue
                ├── ByteArrayValue
                ├── ArrayValue
                │   └── Enum of DataValues
                ├── StructureValue
                │   └── Enum of Fields
                ├── Image
                │   └── Enum
                │       ├── Image (file name)
                │       └── FileType
                └── TimestampValue
                    └── Same Enum as Timestamp
*/


int main() {
    
}
