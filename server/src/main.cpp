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

class IngestionServiceImpl final : public DpIngestionService::Service {
    Status ingestData(ServerContext* context, const IngestDataRequest* datarequest, IngestDataResponse* dataresponse) override {
        // Log metadata
        std::cout << "Provider ID: " << datarequest->providerid() << "\n";
        std::cout << "Client Request ID: " << datarequest->clientrequestid() << "\n";

        // Validate DataFrame
        if (!datarequest->has_ingestiondataframe()) {
            auto* exceptionalResult = dataresponse->mutable_exceptionalresult();
            exceptionalResult->set_message("IngestionDataFrame is missing");
            return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "IngestionDataFrame is Missing");
        }

        const auto& dataFrame = datarequest->ingestiondataframe();

        // Log DataTimestamps
        if (dataFrame.has_datatimestamps() && dataFrame.datatimestamps().has_samplingclock()) {
            const auto& samplingClock = dataFrame.datatimestamps().samplingclock();
            const auto& start_time = samplingClock.starttime();
            std::cout << "Start Time (seconds): " << start_time.epochseconds() << "\n";
            std::cout << "Start Time (nanoseconds): " << start_time.nanoseconds() << "\n";
            std::cout << "Sample Period (nanoseconds): " << samplingClock.periodnanos() << "\n";
            std::cout << "Number of Samples: " << samplingClock.count() << "\n";
        }

        // Log DataColumn values
        for (const auto& column : dataFrame.datacolumns()) {
            std::cout << "DataColumn: " << column.name() << "\n";
            for (const auto& value : column.datavalues()) {
                if (value.has_intvalue()) {
                    std::cout << "  IntValue: " << value.intvalue() << "\n";
                }
            }
        }

        // Respond with AckResult
        auto* ackResult = dataresponse->mutable_ackresult();
        ackResult->set_numrows(dataFrame.datatimestamps().samplingclock().count());
        ackResult->set_numcolumns(dataFrame.datacolumns_size());

        return Status::OK;
    }
};

void RunServer() {
    std::string server_address("0.0.0.0:50051");
    IngestionServiceImpl service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

int main(int argc, char* argv[]) {
    RunServer();
    return 0;
}

