#include "ingestion.grpc.pb.h"
#include "ingestion.pb.h"

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


// TODO update the set methods for grpc objects

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
namespace dp{
	namespace service{
		namespace ingestion{


class OspreyClient {
public:
    OspreyClient(std::shared_ptr<Channel> channel) : stub_(DpIngestionService::NewStub(channel)) {}

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

int main() {
    std::string server_address("localhost:50051");
    OspreyClient client(grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials()));

    // Parse ADC values
    PacketParser parser("path/to/file.dat");
    parser.parseFile();
    const std::vector<int32_t>& adcValues = parser.getAdcValues();

    // Create IngestDataRequest
    IngestDataRequest request;
    request.set_providerid(1);
    request.set_clientrequestid("0001");

    auto* requestTime = request.mutable_requesttime();
    requestTime->set_seconds(1633049600);
    requestTime->set_nanos(123456789);

    auto* dataFrame = request.mutable_ingestiondataframe();
    auto* timestamps = dataFrame->mutable_datatimestamps()->mutable_samplingclock();
    timestamps->set_starttime_seconds(1633049600);
    timestamps->set_starttime_nanos(123456789);
    timestamps->set_periodnanos(1000);
    timestamps->set_numsamples(adcValues.size());

    auto* dataColumn = dataFrame->add_datacolumns();
    dataColumn->set_name("ADC_Channel");
    for (int32_t value : adcValues) {
        auto* dataValue = dataColumn->add_values();
        dataValue->set_intvalue(value);
    }

    client.ingestData(request);

    return 0;
}
}
}
}

