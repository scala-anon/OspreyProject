#include "ingestion.grpc.pb.h"

#include <grpc++/grpc++.h>
#include <grpc/grpc.h>
#include <grpcpp/server_builder.h>

#include <iostream>
#include <memory>
#include <string>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using dp::service::ingestion::DpIngestionService;
using dp::service::ingestion::IngestDataRequest;
using dp::service::ingestion::IngestDataResponse;

//TODO build method to read in data from michael's data file then when finishing the 
// ingestion service implementation when giving the server the data it needs to should 
// be the data that was read in can do a simple loop to read in bytes of data then send them to the server

// potential of what mine should look like
class IngestionServiceImpl final : public DpIngestionService::Service{
    Status ingestData(ServerContext* context, const IngestDataRequest* datarequest, IngestDataResponse* dataresponse) override{
        
        return Status::OK;
    }

/*    Status ingestData(ServerContext* context, const IngestDataRequest* datarequest, ServerWriter<IngestDataResponse>* writer) override{
        
    }
*/
};



void RunServer(){
    std::string server_address("0.0.0.0:50051");
    IngestionServiceImpl service;
    
    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}
int main(int argc, char* argv[])
{
    RunServer();
    
    return 0;
}
