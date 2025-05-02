#include "PacketParser.h"
#include "common.pb.h"
#include "ingestion.pb.h"

#include <grpc/grpc.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <iostream>
#include <string>
#include <cstdint>
#include <chrono>

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;
using DpIngestionService::RegisterProviderRequest;
using DpIngestionService::RegisterProviderResponse;
using DpIngestionService::RegistrationResult;
using DpIngestionService::IngestDataRequest;
using DpIngestionService::IngestionDataFrame;
using DpIngestionService::IngestDataResponse;
using DpIngestionService::AckResult;
namespace dp {
namespace service {
namespace ingestion {

    Attribute makeAtrribute(const std::string name, const std::string value){
        Attribute attribute;
        attribute.set_name(name);
        attribute.set_value(value);
        return(attribute);
    }
    Timestamp makeTimestamp(uint64_t epochSeconds, uint64_t nanoseconds){
        Timestamp timeStamp;
        timeStamp.set_epochSeconds(epochSeconds);
        timeStamp.set_nanoseconds(nanoseconds);
        return(timeStamp);
    }
    EventMetadata makeMetadata(const std::string description, auto startTimestamp, auto stopTimestamp){
        EventMetadata eventMetadata;
        eventMetadata.set_description(description);
        eventMetadata.set_startTimestamp(startTimestamp);
        eventMetadata.set_stopTimestamp(stopTimestamp);
        return(eventMetadata);
    }
    TimestampList makeTimestampList(auto timestamps){
        TimestampList timeStampList;
        timeStampList.set_timestamps(timestamps);
        return(timeStamplist);
    }
SamplingClock makeSamplingClock(auto startTime, uint64_t periodNanos, uint32_t count){
	SamplingClock samplingClock;
    samplingClock.set_startTime(startTime);
    samplingClock.set_periodNanos(periodNanos);
    samplingClock.set_count(count);
    return(samplingClock);
}



    
    IngestionDataFrame makeIngestionDataFrame(auto dataTimestamps, auto dataColumns){

    }
    RegisterProviderRequest makeRegisterProviderRequest(const std::string providerName, auto attributes, auto requestTime){
        RegisterProviderRequest providerRequest;
        providerRequest.set_providername(providerName);
        providerRequest.set_attributes(attributes);
        providerRequest.set_requestTime(requestTime);
        return(providerRequest);
    } 
    
    RegisterProviderResponse makeRegisterProviderResponse(auto responseTime){
        RegisterProviderResponse providerResponse;
        providerResponse.set_responseTime(responseTime);
        return(providerResponse);
    }

    RegistrationResult makeRegResult(uint32_t providerId){
        RegistrationResult regResult;
        regResult.set_providerId(providerId);
        return(regResult);
    } 
    IngestDataRequest makeDataRequest(uint32_t providerId,const std::string clientRequestId, auto requestTime, auto attributes, auto eventMetadata, auto ingestionDataframe){
        IngestDataRequest dataRequest;
        dataRequest.set_providerId(providerId);
        dataRequest.set_clientRequestId(clientRequestId);
        dataRequest.set_requestTime(requestTime);
        dataRequest.set_attributes(attributes);
        dataRequest.set_eventMetadata(eventMetadata);
        dataRequest.mutable_ingestionDataFrame()->CopyFrom(makeDataFrame(dataTimestamps,dataColumns));
        return(dataRequest);
    }

    IngestDataResponse makeDataResponse(uint32_t providerId,const std::string clientRequestId, auto responseTime){
        IngestDataResponse dataResponse;
        dataResponse.set_providerId(provierId);
        dataResponse.set_clientRequestId(clientRequestId);
        dataResponse.set_responseTime(responseTime);
        return(dataResponse);
    }
    AckResult makeAck(uint32_t numRows, uint32_t numColumns){
        AckResult ackRes;
        ackRes.set_numRows(numRows);
        ackRes.set_numColumns(numColumns);
        return(ackRes);
    }

class OspreyClient {
public:
    OspreyClient(const std::string& server_address) 
        : stub_(DpIngestionService::NewStub(grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials()))){}

    void registerProvider{

    std::string ingestData(const IngestDataRequest& request) {
        IngestDataResponse response;
        ClientContext context;

        Status status = stub_->ingestData(&context, request, &response);

        if (status.ok()) {
            if (response.has_ackresult()) {
                std::cout << "Ack Result: Rows=" << response.ackresult().numrows()
                          << ", Columns=" << response.ackresult().numcolumns() << std::endl;
                return "IngestData Success";
            } else if (response.has_exceptionalresult()) {
                std::cerr << "Exceptional Result: " << response.exceptionalresult().message() << std::endl;
                return "IngestData Failed";
            }
        } else {
            std::cerr << status.error_code() << ": " << status.error_message() << std::endl;
            return "RPC Failed";
        }
    }
    RegisterProviderResponse sendRegisterProvider(const RegisterProviderRequest& request){
        RegisterProviderResponse response;
        ClientContext context;
        
        Status status = stub_->registerProvider(&context, request, &response);
        if (status.ok()){
            std::cout << "RegisterProvider successful. Provider ID: "
                      << response.providerid() << std::endl;
        }else{
            std::cerr << "RegisterProvider RPC failed: "
                      << status.error_code() << ": " << status.error_message() << std::endl;
            throw std::runtime_error("RegisterProvider RPC failed");
        }
    }

private:
    std::unique_ptr<DpIngestionService::Stub> stub_;
};

}
}
}

int main() {
    using namespace dp::service::ingestion;

    std::string server_address("localhost:50051");
    OspreyClient client(server_address);
    
    RegisterProviderRequest regRequest;
    regRequest.mutable_providername()->set_providername("Nick");
    
    RegisterProviderRequest regResponse;
    try{
        regResponse = client.sendRegisterProvider(regRequest);
    } catch (const std::exception& e){
        std::cerr << "Failed to register provider: " << e.what() << std::endl;
        return 1;
    }
    
    PacketParser parser("data/mic1-8-CH17-20240511-121442.dat");
    parser.parseFile();
    const std::vector<int32_t>& adcValues = parser.getAdcValues();

    // Define the sample rate and number of samples
    constexpr size_t sampleCount = 250000;
    
    // Get valid provider ID
    std::string providerId = regResponse.provider().providerid()

    // Create IngestDataRequest
    IngestDataRequest request;
    request.set_providerid(providerId);
    request.set_clientrequestid("0002");

    auto* dataFrame = request.mutable_ingestiondataframe();
    auto* timestamps = dataFrame->mutable_datatimestamps();
    auto* samplingClock = timestamps->mutable_samplingclock();
    auto* startTime = samplingClock->mutable_starttime();
    
    uint64_t epochSeconds = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    uint64_t nanoSeconds = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    startTime->set_epochseconds(epochSeconds);
    startTime->set_nanoseconds(nanoSeconds);
    
    // For a 250k sample per second rate, each sample is 1e9/250000 = 4000 ns apart
    long double  periodNano = 4000;
    samplingClock->set_periodnanos(periodNano);
    samplingClock->set_count(sampleCount);

    auto* dataColumn1 = dataFrame->add_datacolumns();
    dataColumn1->set_name("ADC");
    auto* dataColumn2 = dataFrame->add_datacolumns();
    dataColumn2->set_name("TimeStamp(Nanoseconds)");

    for(int i = 0; i <= sampleCount; i++){
        // Record the current timestamp for this sample
        uint64_t timeStamp = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        
        // Add timestamp to the second column
        auto* timeStampValues = dataColumn2->add_datavalues();
        timeStampValues->set_intvalue(timeStamp);
        
        // Add the corresponding ADC value to the first data column
        auto* dataValues = dataColumn1->add_datavalues();
         dataValues->set_intvalue(adcValues[i]);
    }
    std::cout << "DEBUG: Provider ID set to: " << request.providerid() << std::endl;
    client.ingestData(request);

    return 0;
}


