#include <ingestion.pb.h>
#include <ingestion.grpc.pb.h>

#include <grpc/grpc.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <cstdint>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using dp::service::ingestion::DpIngestionService;
using dp::service::ingestion::RegisterProviderResponse_RegistrationResult;
using dp::service::ingestion::IngestDataResponse_AckResult;
using dp::service::ingestion::RegisterProviderResponse;
using dp::service::ingestion::RegisterProviderRequest;
using dp::service::ingestion::IngestDataResponse;
using dp::service::ingestion::IngestDataRequest_IngestionDataFrame;
using dp::service::ingestion::IngestDataRequest;

/*
    create RegisterProviderRequest message
    message will need providerName, attributes, and requestTime
    send message to rpc registerProvider this returns RegisterProviderResponse
    create IngestDataRequest message
    message will need attributes(optional), clientRequestId, requestTime, eventMetadata(optional), ingestDataFrame, providerId 
    send message to ingestData this returns IngestDataResponse 
*/

// This function fills in a register request message based on user input and then return the message
dp::service::ingestion::RegisterProviderRequest promptForProviderRequest() {
    dp::service::ingestion::RegisterProviderRequest providerRequest;
    
    std::cout << "Enter the provider name: ";
    std::string providerName;
    std::getline(std::cin, providerName);

    if (!providerName.empty()) {
        providerRequest.set_providername(providerName);
    }
    // add section for attributes (if needed)

    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
    providerRequest.set_requesttime(now_ms);

    return providerRequest;
}

void callRegisterProviderRpc(grpc::ClientContext& context, 
                             std::shared_ptr<dp::service::ingestion::DpIngestionService::Stub> stub) {
    // Create and populate the request message
    dp::service::ingestion::RegisterProviderRequest request = promptForProviderRequest();
    
    // Create the response message
    dp::service::ingestion::RegisterProviderResponse response;
    
    // Make the RPC call
    grpc::Status status = stub->RegisterProvider(&context, request, &response);
    
    // Handle the response
    if (status.ok()) {
        std::cout << "RPC succeeded" << std::endl;
        // Process the response if needed
    } else {
        std::cerr << "RPC failed: " << status.error_message() << std::endl;
    }
}

// This function fills in a ingest data request message based on user input and then return the message
dp::service::ingestion::IngestDataRequest promptForIngestData() {
    dp::service::ingestion::IngestDataRequest DataRequest;
    
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

    dp::service::ingestion::IngestDataRequest_IngestionDataFrame dataFrame = promptForIngestionDataFrame();
    DataRequest.set_ingestiondataframe(dataFrame);

    return DataRequest;
}

dp::service::ingestion::IngestDataRequest_IngestionDataFrame promptForIngestionDataFrame() {
    dp::service::ingestion::IngestDataRequest_IngestionDataFrame DataFrame;

    // gets requestTime value
    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
    DataFrame.set_requesttime(now_ms);

    // add section for DataColumn

    return DataFrame;
}

void callIngestDataRpc(grpc::ClientContext& context, 
                      std::shared_ptr<dp::service::ingestion::DpIngestionService::Stub> stub) {
    // Create and populate the request message
    dp::service::ingestion::IngestDataRequest request = promptForIngestData();
    
    // Create the response message
    dp::service::ingestion::IngestDataResponse response;
    
    // Make the RPC call
    grpc::Status status = stub->IngestData(&context, request, &response);
    
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
    auto stub = dp::service::ingestion::DpIngestionService::NewStub(channel);

    // Set up the gRPC client context
    grpc::ClientContext context;
    
    // Call the RPC methods
    callRegisterProviderRpc(context, stub);
    callIngestDataRpc(context, stub);

    return 0;
}