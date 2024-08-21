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

using namespace dp::service::ingestion;
using namespace google::protobuf::internal;

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

/*
    create RegisterProviderRequest message
    message will need providerName, attributes, and requestTime
    send message to rpc registerProvider this returns RegisterProviderResponse
    create IngestDataRequest message
    message will need attributes(optional), clientRequestId, requestTime, eventMetadata(optional), ingestDataFrame, providerId 
    send message to ingestData this returns IngestDataResponse 
*/

// This function fills in a register request message based on user input and then returns the message
RegisterProviderRequest promptForProviderRequest() {
    RegisterProviderRequest providerRequest;
    
    std::cout << "Enter the provider name: ";
    std::string providerName;
    std::getline(std::cin, providerName);

    if (!providerName.empty()) {
        providerRequest.set_providername(providerName);
    }
    // add section for attributes (if needed)

    auto now = std::chrono::system_clock::now();
    auto now_nano = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
    providerRequest.set_requesttime(now_nano);

    return providerRequest;
}

void callRegisterProviderRpc(ClientContext& context, 
                             std::shared_ptr<DpIngestionService::Stub> stub) {
    // Create and populate the request message
    RegisterProviderRequest request = promptForProviderRequest();
    
    // Create the response message
    RegisterProviderResponse response;
    
    // Make the RPC call
    Status status = stub->RegisterProvider(&context, request, &response);
    
    // Handle the response
    if (status.ok()) {
        std::cout << "RPC succeeded" << std::endl;
        // Process the response if needed
    } else {
        std::cerr << "RPC failed: " << status.error_message() << std::endl;
    }
}

// This function fills in an ingest data request message based on user input and then returns the message
IngestDataRequest promptForIngestData() {
    IngestDataRequest DataRequest;
    
    // sets providerId 
    uint32_t providerIdVal = 1;
    DataRequest.set_providerid(providerIdVal);
    
    // sets clientRequestId 
    std::string fileName = "counter.txt";
    int counter = 1;

    std::ifstream infile(fileName);
    if(infile.is_open()){
        infile >> counter;
        infile.close();
    }
    counter++;
    std::stringstream ss;
    ss << std::setw(4) << std::setfill('0') << counter;
    std::string counterStr = ss.str();
    
    DataRequest.set_clientrequestid(counterStr);
    
    std::ofstream outfile(filename);
    if (outfile.is_open()) {
        outfile << counter;
        outfile.close();
    }


    // sets requestTime 
    auto now = std::chrono::system_clock::now();
    auto now_nano = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
    DataRequest.set_requesttime(now_nano);

    // sets dataFrame 
    IngestDataRequest_IngestionDataFrame dataFrame = promptForIngestionDataFrame();
    DataRequest.set_ingestiondataframe(dataFrame);

    return DataRequest;
}

IngestDataRequest_IngestionDataFrame promptForIngestionDataFrame() {
    IngestDataRequest_IngestionDataFrame DataFrame;

    // gets requestTime value
    auto now = std::chrono::system_clock::now();
    auto now_nano = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
    DataFrame.set_requesttime(now_nano);

    // add section for DataColumn

    return DataFrame;
}

DataTimestamps SetDataTimeStamps(){
    DataTimestamps dataTimeStamp;
    SamplingClock clock = SetSamplingClock();

    // sets samplingclock
    dataTimeStamp.set_allocated_samplingclock(clock);

    //TODO add set method for timestamplist
}

SamplingClock SetSamplingClock(){
    SamplingClock clock;
    TimeStamp time = SetTimeStamp;
    std::string fileName = "";
    std::vector<uint64_t> periodNano = measureLineReadTime(fileName);

    //TODO make method that will read the number of samples in the contained interval   
    uint32_t counter = 3;
    
    // set startTime
    clock.set_allocated_starttime(time);

    // set periodNanos
    for(uint64_t number : periodNano){
        clock.set_periodnanos(number);
    }

    // set count
    clock.set_count(counter);
    return(clock);
}

Timestamp SetTimeStamp(){
    Timestamp clock;
    // Get the current time point
    uint64_t now = std::chrono::system_clock::now();

    // Get the number of seconds since the Unix epoch
    uint64_t epoch_seconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

    // Get the number of nanoseconds since the Unix epoch
    uint64_t epoch_nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();

    // Calculate the number of nanoseconds within the current second
    uint64_t nanoseconds_within_second = epoch_nanoseconds - (epoch_seconds * 1000000000LL);
    
    time.set_epochseconds(epoch_seconds);
    time.set_nanoseconds(nanoseconds_within_second);
    return(time);
}

std::vector<uint64_t> measureLineReadTime(const std::string& filename) {
    // Open the file
    std::ifstream file(filename);
    std::vector<uint64_t> lineReadTimes;

    if (!file.is_open()) {
        std::cerr << "Could not open the file: " << filename << std::endl;
        return lineReadTimes;  // Return an empty vector if the file couldn't be opened
    }

    std::string line;

    // Read each line and measure the time taken
    while (std::getline(file, line)) {
        // Start time
        auto start = std::chrono::high_resolution_clock::now();

        // Here we simulate reading the line, but it's already read by std::getline
        // Do any additional processing with the line here if necessary

        // End time
        auto end = std::chrono::high_resolution_clock::now();

        // Calculate the time taken in nanoseconds
        uint64_t periodNanos = static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
        );

        // Store the time taken for this line
        lineReadTimes.push_back(periodNanos);
    }

    file.close();
    return lineReadTimes;  // Return the vector of times
}

DataColumn SetDataColumn(){
    DataColumn column;
    
}


void callIngestDataRpc(ClientContext& context, 
                      std::shared_ptr<DpIngestionService::Stub> stub) {
    // Create and populate the request message
    IngestDataRequest request = promptForIngestData();
    
    // Create the response message
    IngestDataResponse response;
    
    // Make the RPC call
    Status status = stub->IngestData(&context, request, &response);
    
    // Handle the response
    if (status.ok()) {
        std::cout << "RPC succeeded" << std::endl;
        // Process the response if needed
    } else {
        std::cerr << "RPC failed: " << status.error_message() << std::endl;
    }
}

int main(int argc, char* argv[])
{
    auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
    auto stub = DpIngestionService::NewStub(channel);

    // Set up the gRPC client context
    ClientContext context;
    
    // Call the RPC methods
    callRegisterProviderRpc(context, stub);
    callIngestDataRpc(context, stub);

    return 0;
}
