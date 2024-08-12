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
    
    // gets providerId value
    std::cout << "Enter the provider id: ";
    std::string providerIdIn;
    std::getline(std::cin, providerIdIn);
    uint32_t providerIdVal;
    
    try {
        size_t pos;
        unsigned long ulValue = std::stoul(providerIdIn, &pos);

        if (pos != providerIdIn.length() || ulValue > std::numeric_limits<uint32_t>::max()) {
            throw std::out_of_range("Input out of range or invalid");
        }
        providerIdVal = static_cast<uint32_t>(ulValue);
        DataRequest.set_providerid(providerIdVal);

    } catch (const std::invalid_argument& e) {
        std::cerr << "Invalid input. Please enter a valid positive integer." << std::endl;
    } catch (const std::out_of_range& e) {
        std::cerr << "Input out of range for uint32_t." << std::endl;
    }
    
    // gets clientRequestId value
    std::cout << "Enter the client request id: ";
    std::string clientId;
    std::getline(std::cin, clientId);

    if (!clientId.empty()) {
        DataRequest.set_clientrequestid(clientId);
    }

    // gets requestTime value
    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
    DataRequest.set_requesttime(now_ms);

    IngestDataRequest_IngestionDataFrame dataFrame = promptForIngestionDataFrame();
    DataRequest.set_ingestiondataframe(dataFrame);

    return DataRequest;
}

IngestDataRequest_IngestionDataFrame promptForIngestionDataFrame() {
    IngestDataRequest_IngestionDataFrame DataFrame;

    // gets requestTime value
    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
    DataFrame.set_requesttime(now_ms);

    // add section for DataColumn

    return DataFrame;
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
