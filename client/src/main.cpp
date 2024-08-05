#include <ingestion.pb.h>
#include <ingestion.grpc.pb.h>

#include <grpc/grpc.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <iostream>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using dp::service::ingestion::DpIngestionService;
using dp::service::ingestion::RegisterProviderRequest;
using dp::service::ingestion::RegisterProviderResponse;
using dp::service::ingestion::IngestDataRequest;
using dp::service::ingestion::IngestDataResponse;



class IngestionClient {
public:
    IngestionClient(std::shared_ptr<Channel> channel)
        : stub_(DpIngestionService::NewStub(channel)) {}

    bool getOneData(const IngestDataRequest& datarequest, IngestDataResponse* dataresponse) {
        ClientContext context;
        Status status = stub_->ingestData(&context, datarequest, dataresponse);
        if (!status.ok()) {
            std::cerr << "getOneData failed: " << status.error_message() << std::endl;
            return false;
        }
        return true;
    }

private:
    std::unique_ptr<DpIngestionService::Stub> stub_;
};
int main(int argc, char* argv[])
{
    auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
    IngestionClient client(channel);
    
    IngestDataRequest datarequest;
    IngestDataResponse dataresponse;

    // Set fields for datarequest using setter methods
    datarequest.set_providerid("providerId");
    datarequest.set_clientrequestid("clientRequestId");
    datarequest.set_requesttime("requestTime");
    datarequest.set_attributes("attributes");
    datarequest.set_eventmetadata("eventMetaData");
    datarequest.set_ingestiondataframe("ingestionDataFrame");
    
    // Set fields for dataresponse using setter methods
    dataresponse.set_providerid("providerId");
    dataresponse.set_clientrequestid("clientRequestId");
    dataresponse.set_responsetime("responseTime");
    
    // datarequest = ("providerId","clientRequestId","requestTime","attributes","eventMetaData", "ingestionDataFrame");
    // dataresponse = ("providerId","clientRequestId","responseTime");
    if (client.getOneData(datarequest, &dataresponse)) {
        std::cout << "Data Response - Provider ID: " << dataresponse.providerid() << std::endl;
        std::cout << "Data Response - Client Request ID: " << dataresponse.clientrequestid() << std::endl;
        std::cout << "Data Response - Response Time: " << dataresponse.responsetime() << std::endl;
    }
     
    return 0;
}
