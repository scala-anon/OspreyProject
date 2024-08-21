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
#include <nlohmann/json.hpp> // Include the nlohmann/json.hpp for JSON parsing since hdr file is close to json format

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
    note: build this in reverse order when coding so that it builds
*/

void DataValue SetDataValue(){
    DataValue values;
    vector<Signal> signals = parseSignals("/home/nick/Documents/data/mic1-8.hdr");
    // need to check status meaning need to build data value status message 
    values.set_bytearrayvalue(signals);
    for (size_t i = 0; i < signals.size(); ++i) {
    std::cout << "Signal Index: " << i << '\n'
                  << "  Name: " << sig.name << '\n'
                  << "  Chassis: " << sig.chassis << '\n'
                  << "  Channel: " << sig.channel << '\n'
                  << "  Egu: " << sig.egu << '\n'
                  << "  Slope: " << sig.slope << '\n'
                  << "  Intercept: " << sig.intercept << '\n'
                  << "  Coupling: " << sig.coupling << '\n'
                  << "  SigNum: " << sig.sigNum << '\n'
                  << "  Description: " << sig.description << '\n'
                  << "  Response Node: " << sig.responseNode << '\n'
                  << "  Response Direction: " << sig.responseDirection << '\n'
                  << "  Reference Node: " << sig.referenceNode << '\n'
                  << "  Reference Direction: " << sig.referenceDirection << '\n'
                  << std::endl;
    }
    
}

// Define the struct to hold the signal values
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

// Define an enum for easy access to specific signal values
enum SignalValue {
    CHASSIS,
    CHANNEL,
    NAME,
    EGU,
    SLOPE,
    INTERCEPT,
    COUPLING,
    SIG_NUM,
    DESCRIPTION,
    RESPONSE_NODE,
    RESPONSE_DIRECTION,
    REFERENCE_NODE,
    REFERENCE_DIRECTION
};

// Function to parse the JSON file and return a vector of Signal structs
vector<Signal> parseSignals(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open the file.\n";
        return {};
    }

    stringstream buffer;
    buffer << file.rdbuf();
    string content = buffer.str();
    file.close();

    nlohmann::json jsonData = nlohmann::json::parse(content);

    vector<Signal> signals;

    // Loop through the Signals array in the JSON data
    for (const auto& signal : jsonData["Signals"]) {
        Signal sig;
        sig.chassis = signal["Address"]["Chassis"];
        sig.channel = signal["Address"]["Channel"];
        sig.name = signal["Name"];
        sig.egu = signal["Egu"];
        sig.slope = signal["Slope"];
        sig.intercept = signal["Intercept"];
        sig.coupling = signal["Coupling"];
        sig.sigNum = signal["SigNum"];
        sig.description = signal["Desc"];
        sig.responseNode = signal["ResponseNode"];
        sig.responseDirection = signal["ResponseDirection"];
        sig.referenceNode = signal["ReferenceNode"];
        sig.referenceDirection = signal["ReferenceDirection"];

        // Store the Signal struct in the vector
        signals.push_back(sig);
    }

    return signals;
}

void DataColumn 





int main(int argc, char* argv[])
{
    auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
    auto stub = DpIngestionService::NewStub(channel);

    // Set up the gRPC client context
    ClientContext context;
    

    return 0;
}
