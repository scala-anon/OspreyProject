#include <ingestion.pb.h>
#include <ingestion.grpc.pb.h>
#include <common.pb.h>

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


// TODO add using statements

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
namespace dp{
	namespace service{
		namespace ingestion{


class OspreyClient {
	public:
		OspreyClient(std::shared_ptr<Channel> channel) : _stub{DpIngestionService::NewStub(channel)}{}
		std::string registerProvider(const std::string& providerName){
		RegisterProviderRequest request;
		request.set_providername(providerName);
		RegisterProviderResponse response;
		ClientContext context;
		Status status;
		status = _stub->registerProvider(&context, request, &response);

		if(status.ok()){
			//TODO add the timestamp parameter for response time i.e enum:int
			const ::Timestamp& time = response.responsetime();
			std::cout << time.nanoseconds() << std::endl;
			return "time";
		}else{
			std::cerr << status.error_code() << ": " << status.error_message() << std::endl;
			return "RPC Failed";
		}
		}
	private: 
		std::unique_ptr<DpIngestionService::Stub> _stub;

};

int main(int argc, char** argv){
	std::string server_address("localhost:5001");
	OspreyClient client{grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials())};
	std::string providerName{"Nick"};
	std::string responseTime = client.registerProvider(providerName);
        std::cout << "Client Received: " << providerName << std::endl;
	return 0; 
}
}
}
}
