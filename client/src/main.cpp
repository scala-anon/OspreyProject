#include "PacketParser.h"
#include "ingestion.grpc.pb.h"
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
using grpc::Status;
namespace dp {
namespace service {
namespace ingestion {

class OspreyClient {
public:
    OspreyClient(const std::string& server_address) 
        : stub_(DpIngestionService::NewStub(grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials()))){}

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

    PacketParser parser("data/mic1-8-CH17-20240511-121442.dat");
    parser.parseFile();
    const std::vector<int32_t>& adcValues = parser.getAdcValues();

    // Define the sample rate and number of samples
    constexpr size_t sampleCount = 250000;
    
    // Create IngestDataRequest
    IngestDataRequest request;
    request.set_providername("Nick");
    request.set_providerid(1);
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


