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
using dp::service::ingestion::RegisterProviderResponse_RegistrationResult;
/* message(providerId)
providerId: clear_providerId(), set_providerid(), _internal_set_providerid(),
*/
using dp::service::ingestion::IngestDataResponse_AckResult;
/* message(numRows, numColumns)
numRows: clear_numrows(), set_numrows(), _internal_set_numrows()

numColumns: clear_numcolumns(), set_numcolumns(), _internal_set_numcolumns()
*/
using dp::service::ingestion::RegisterProviderResponse;
/* message(responseTime, exceptionalResult, registrationResult)
responseTime: has_responsetime(), clear_responsetime(), set_allocated_responsetime(), 
unsafe_arena_set_allocated_resppnsetime()

exceptionalResult: has_exceptionalresult(), _internal_has_exceptionalresult(), clear_exceptionalresult(),
set_allocated_exceptionresult(), unsafe_arena_set_allocated_exceptionalresult(),

registrationResult: has_registrationresult(), _internal_has_registrationresult(), clear_registrationresult(),
set_allocated_registrationresult(), unsage_arena_set_allocated_registrationresult(),
*/
using dp::service::ingestion::RegisterProviderRequest;
/* message(providerName, attributes, requestTime)
providerName: clear_providername(), set_providername(), set_allocated_providername(), _internal_set_providername()

attributes: clear_attributes(), add_attributes(), attributes_size(), _internal_attributes_size()

requestTime: has_requesttime(), clear_requesttime(), set_allocated_requesttime(), 
unsage_arena_set_allocated_requesttime() 
*/
using dp::service::ingestion::IngestDataResponse;
/* message(clientRequestId, responseTime, providerId, exceptionResult, ackResult)
clientRequestId: clear_clientrequestid(), set_clientrequestid(), set_allocated_clientrequestid(),
_internal_set_clientrequestid()

responseTime: has_responsetime(), clear_responsetime(), set_allocated_responsetime(), 
unsafe_arena_set_allocated_resppnsetime()

providerId: clear_providerId(), set_providerid(), _internal_set_providerid(),

exceptionalResult: has_exceptionalresult(), _internal_has_exceptionalresult(), clear_exceptionalresult(),
set_allocated_exceptionresult(), unsafe_arena_set_allocated_exceptionalresult()

ackResult: has_ackresult(), _internal_has_ackresult(), clear_ackresult(),
set_allocated_ackresult(), unsafe_arena_set_allocated_ackresult()
*/
using dp::service::ingestion::IngestDataRequest_IngestionDataFrame;
/* message(dataColumns, dataTimestamps)
dataColumns: datacolumns_size(), _internal_datacolumns_size(), clear_datacolumns(), add_datacolumns()

dataTimestamps: has_datatimestamps(), clear_datatimestamps(), set_allocated_datatimestamps(),
unsafe_arena_set_allocated_datatimestamps(), 
*/
using dp::service::ingestion::IngestDataRequest;
/* message(attributes, clientRequestId, requestTime, eventMetadata, ingestDataFrame, providerId)
attributes: clear_attributes(), add_attributes(), attributes_size(), _internal_attributes_size()

clientRequestId: clear_clientrequestid(), set_clientrequestid(), set_allocated_clientrequestid(),
_internal_set_clientrequestid()

requestTime: has_requesttime(), clear_requesttime(), set_allocated_requesttime(), 
unsage_arena_set_allocated_requesttime() 

eventMetaData: has_eventmetadata(), clear_eventmetadata(), set_allocated_eventmetadata(), 
unsafe_arena_set_allocated_eventmetadata()

ingestionDataFrame: has_ingestiondataframe(), clear_ingestiondataframe(), set_allocated_ingestiondataframe(),
unsafe_arena_set_allocated_ingestiondataframe()
*/



/*
    create RegisterProviderRequest message
    message will need providerName, attributes, and requestTime
    send message to rpc registerProvider this returns RegisterProviderResponse
    create IngestDataRequest message
    message will need providerId, clientRequestId, requestTime, attributes(optional), eventMetaData(optional), and ingestionDataFrame 
    send message to ingestData this returns IngestDataResponse 
*/

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
