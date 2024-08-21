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
    Purpose of this file is to follow my pseudo to a tee to have a more clear understanding
    1. build Ingestion Data frame request
        2. request needs data columns and time stamps
    3. build timestamp message return this message
        4. message needs sampling clock and time stamp list
    5. build sampling clock message
        6. message needs start time, periodnanos, and count
        7. return message
    8. build time stamp list message
        9. message needs time stamps
        10. return message
    11. build data column message
        12. message needs data values and name
    13. build data value 
        14. message needs plethora of values
        15. read in header file to file values
    14. will need to build a message for the data ingestion values 
    note: build this in reverse order when coding so that it builds
*/

struct Signal {
    int chassis;
    int channel;
    string name;
    string egu;
    double slope;
    double intercept;
    string coupling;
    int sigNum;
    string description;
    int responseNode;
    int responseDirection;
    int referenceNode;
    int referenceDirection;
};

// Struct to hold status information
struct Status {
    string message;
    int statusCode;  // StatusCode enum value
    int severity;    // Severity enum value
};

DataValue BuildDataValue(const Signal& sig, const string& type, const Status& status) {
    DataValue dataValue;
    
    if (type == "string") {
        dataValue.set_stringvalue(sig.name);
    } else if (type == "boolean") {
        // Assume a boolean value for demonstration
        dataValue.set_booleanvalue(true);
    } else if (type == "uint") {
        dataValue.set_uintvalue(sig.chassis);
    } else if (type == "ulong") {
        dataValue.set_ulongvalue(static_cast<uint64_t>(sig.channel));
    } else if (type == "int") {
        dataValue.set_intvalue(sig.sigNum);
    } else if (type == "long") {
        dataValue.set_longvalue(static_cast<int64_t>(sig.referenceNode));
    } else if (type == "float") {
        dataValue.set_floatvalue(static_cast<float>(sig.slope));
    } else if (type == "double") {
        dataValue.set_doublevalue(sig.intercept);
    } else if (type == "bytes") {
        // Set byte array value if needed
        string bytesValue; // Set to some byte data
        dataValue.set_bytearrayvalue(bytesValue);
    } else if (type == "array") {
        // Build Array value if needed
        Array arrayValue;
        // Add values to arrayValue
        dataValue.set_arrayvalue(arrayValue);
    } else if (type == "structure") {
        // Build Structure value if needed
        Structure structureValue;
        // Add fields to structureValue
        dataValue.set_structurevalue(structureValue);
    } else if (type == "image") {
        // Build Image value if needed
        Image imageValue;
        // Set image value
        dataValue.set_imagevalue(imageValue);
    } else if (type == "timestamp") {
        // Build Timestamp value if needed
        Timestamp timestampValue;
        // Set timestamp value
        dataValue.set_timestampvalue(timestampValue);
    }
    
    // Set the status of the data value
    auto valueStatus = dataValue.mutable_valuestatus();
    valueStatus->set_message(status.message);
    valueStatus->set_statuscode(static_cast<ValueStatus::StatusCode>(status.statusCode));
    valueStatus->set_severity(static_cast<ValueStatus::Severity>(status.severity));
    
    return dataValue;
}

DataColumn BuildDataColumn(const string& name, const vector<DataValue>& dataValues) {
    DataColumn dataColumn;
    dataColumn.set_name(name);
    for (const auto& dataValue : dataValues) {
        *dataColumn.add_datavalues() = dataValue;
    }
    return dataColumn;
}


int main() {
    // Example data
    Signal sig = {1, 2, "Sensor1", "Egu1", 0.1, 0.01, "Coupling1", 100, "Desc1", 1, 1, 1, 1};
    Status status = {"No error", ValueStatus::NO_STATUS, ValueStatus::NO_ALARM};

    // Build DataValue
    DataValue dataValue = BuildDataValue(sig, "string", status);

    // Build DataColumn
    vector<DataValue> dataValues = {dataValue};
    DataColumn dataColumn = BuildDataColumn("item1", dataValues);

    // Print DataColumn details
    cout << "Data Column Name: " << dataColumn.name() << endl;
    for (const auto& value : dataColumn.datavalues()) {
        if (value.has_stringvalue()) {
            cout << "Data Value: " << value.stringvalue() << endl;
        }
        // Handle other types similarly
    }

    return 0;
}
