// --- Header Inclusions ---
#include "PacketParser.h"              // Custom header for parsing .dat files into ADC values
#include "common.pb.h"                 // Protobuf definitions for shared/common messages
#include "ingestion.pb.h"              // Protobuf definitions for ingestion request/response messages
#include "ingestion.grpc.pb.h"         // gRPC-generated stub definitions

#include <grpc/grpc.h>                 // Core gRPC functionality
#include <grpcpp/create_channel.h>     // gRPC channel creation
#include <grpcpp/security/credentials.h> // Security credentials for channel
#include <grpcpp/client_context.h>     // Context for gRPC client
#include <iostream>                    // Standard I/O stream
#include <string>                      // String handling
#include <vector>                      // STL vector
#include <chrono>                      // Time utilities

// --- Namespaces ---
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

// Shorthand for the namespace used in the proto file
namespace dping = dp::service::ingestion;

// --- Helper: Create a Timestamp protobuf object ---
Timestamp makeTimeStamp(uint64_t epoch, uint64_t nano) {
    Timestamp ts;
    ts.set_epochseconds(epoch);
    ts.set_nanoseconds(nano);
    return ts;
}

// --- Helper: Create an Attribute protobuf object ---
Attribute makeAttribute(const std::string& name, const std::string& value) {
    Attribute attr;
    attr.set_name(name);
    attr.set_value(value);
    return attr;
}

// --- Helper: Create EventMetadata with timestamps ---
EventMetadata makeEventMetadata(const std::string& desc, uint64_t startEpoch, uint64_t startNano,
                                uint64_t endEpoch, uint64_t endNano) {
    EventMetadata metadata;
    metadata.set_description(desc);
    *metadata.mutable_starttimestamp() = makeTimeStamp(startEpoch, startNano);
    *metadata.mutable_stoptimestamp() = makeTimeStamp(endEpoch, endNano);
    return metadata;
}

// --- Helper: Create SamplingClock with start time and period ---
SamplingClock makeSamplingClock(uint64_t epoch, uint64_t nano, uint64_t periodNanos, uint32_t count) {
    SamplingClock clock;
    *clock.mutable_starttime() = makeTimeStamp(epoch, nano);
    clock.set_periodnanos(periodNanos);
    clock.set_count(count);
    return clock;
}

// --- Helpers: Create DataValue protobufs for different types ---
DataValue makeDataValueWithSInt32(int val) {
    DataValue dv;
    dv.set_intvalue(val);
    return dv;
}

DataValue makeDataValueWithUInt64(uint64_t val) {
    DataValue dv;
    dv.set_ulongvalue(val);
    return dv;
}

DataValue makeDataValueWithTimestamp(uint64_t sec, uint64_t nano) {
    DataValue dv;
    *dv.mutable_timestampvalue() = makeTimeStamp(sec, nano);
    return dv;
}

// --- Helper: Construct a DataColumn from a list of DataValues ---
DataColumn makeDataColumn(const std::string& name, const std::vector<DataValue>& values) {
    DataColumn col;
    col.set_name(name);
    for (const auto& val : values) {
        *col.add_datavalues() = val;
    }
    return col;
}

// --- Helper: Construct an IngestionDataFrame with sampling clock and data columns ---
dping::IngestDataRequest::IngestionDataFrame makeIngestionDataFrame(
    const SamplingClock& samplingClock, const std::vector<DataColumn>& dataColumns) {
    dping::IngestDataRequest::IngestionDataFrame frame;
    *frame.mutable_datatimestamps()->mutable_samplingclock() = samplingClock;
    for (const auto& col : dataColumns) {
        *frame.add_datacolumns() = col;
    }
    return frame;
}

// --- Helper: Build a complete IngestDataRequest with metadata and data ---
dping::IngestDataRequest makeIngestDataRequest(
    const std::string& providerId,
    const std::string& clientRequestId,
    const std::vector<Attribute>& attributes,
    const std::vector<std::string>& tags,
    const EventMetadata& metadata,
    const SamplingClock& samplingClock,
    const std::vector<DataColumn>& dataColumns) {
    
    dping::IngestDataRequest request;
    request.set_providerid(providerId);
    request.set_clientrequestid(clientRequestId);

    for (const auto& attr : attributes)
        *request.add_attributes() = attr;

    for (const auto& tag : tags)
        request.add_tags(tag);

    *request.mutable_eventmetadata() = metadata;
    *request.mutable_ingestiondataframe() = makeIngestionDataFrame(samplingClock, dataColumns);

    return request;
}

// --- Helper: RegisterProviderRequest builder ---
dping::RegisterProviderRequest makeRegisterProviderRequest(
    const std::string& providerName,
    const std::vector<Attribute>& attributes,
    uint64_t epoch,
    uint64_t nano) {
    
    dping::RegisterProviderRequest request;
    request.set_providername(providerName);

    for (const auto& attr : attributes)
        *request.add_attributes() = attr;

    return request;
}

// --- Helper: Wrap provider ID in RegistrationResult protobuf ---
dping::RegisterProviderResponse::RegistrationResult makeRegistrationResult(const std::string& providerId) {
    dping::RegisterProviderResponse::RegistrationResult result;
    result.set_providerid(providerId);
    return result;
}

// --- Helper: Wrap registration result with response timestamp ---
dping::RegisterProviderResponse makeRegisterProviderResponseWithResult(
    uint64_t epoch,
    uint64_t nano,
    const dping::RegisterProviderResponse::RegistrationResult& result) {

    dping::RegisterProviderResponse response;
    *response.mutable_responsetime() = makeTimeStamp(epoch, nano);
    *response.mutable_registrationresult() = result;
    return response;
}

// --- gRPC Client Wrapper Class ---
class OspreyClient {
public:
    // Constructor: sets up stub for communication with gRPC server
    OspreyClient(const std::string& server_address)
        : stub_(dp::service::ingestion::DpIngestionService::NewStub(
            grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials()))) {}

    // Send a RegisterProvider request and return the response
    dping::RegisterProviderResponse sendRegisterProvider(const dping::RegisterProviderRequest& request) {
        dping::RegisterProviderResponse response;
        grpc::ClientContext context;
        grpc::Status status = stub_->registerProvider(&context, request, &response);

        if (!status.ok()) {
            std::cerr << "RegisterProvider RPC failed: " << status.error_message() << std::endl;
            throw std::runtime_error("RegisterProvider RPC failed");
        }
        return response;
    }

    // Send an IngestData request and handle acknowledgment
    std::string ingestData(const dping::IngestDataRequest& request) {
        dping::IngestDataResponse response;
        grpc::ClientContext context;
        grpc::Status status = stub_->ingestData(&context, request, &response);

        if (status.ok()) {
            if (response.has_ackresult()) {
                std::cout << "Ack Result: Rows=" << response.ackresult().numrows()
                          << ", Columns=" << response.ackresult().numcolumns() << std::endl;
                return "IngestData Success";
            } else {
                std::cerr << "No AckResult in response." << std::endl;
                return "IngestData Failed";
            }
        } else {
            std::cerr << status.error_code() << ": " << status.error_message() << std::endl;
            return "RPC Failed";
        }
    }

private:
    std::unique_ptr<dp::service::ingestion::DpIngestionService::Stub> stub_;  // gRPC stub
};

// --- Main Application Entry Point ---
int main() {
    std::string server_address = "localhost:50051";
    OspreyClient client(server_address);

    // --- Get current system time in seconds and nanoseconds ---
    uint64_t nowSec = std::chrono::system_clock::now().time_since_epoch() / std::chrono::seconds(1);
    uint64_t nowNano = std::chrono::duration_cast<std::chrono::nanoseconds>(
                           std::chrono::system_clock::now().time_since_epoch()).count();

    // --- Send registration request ---
    auto regReq = makeRegisterProviderRequest("Nick", {}, nowSec, nowNano);
    dping::RegisterProviderResponse regResp;
    try {
        regResp = client.sendRegisterProvider(regReq);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    // --- Check registration success ---
    if (!regResp.has_registrationresult()) {
        std::cerr << "Registration failed: no registration result in response." << std::endl;
        return 1;
    }
    std::string providerId = regResp.registrationresult().providerid();

    // --- Parse .dat file into ADC values ---
    PacketParser parser("data/mic1-8-CH17-20240511-121442.dat");
    parser.parseFile();
    const std::vector<int32_t>& adcValues = parser.getAdcValues();

    // --- Construct sampling clock (4000ns between samples) ---
    auto clock = makeSamplingClock(nowSec, nowNano, 4000, 10); // TODO: Replace 10 with adcValues.size()

    // --- Fill ADC and timestamp data ---
    std::vector<DataValue> adcData, timestampData;
    for (size_t i = 0; i < 10; ++i) { // TODO: Replace 10 with adcValues.size()
        adcData.push_back(makeDataValueWithSInt32(adcValues[i]));
        timestampData.push_back(makeDataValueWithTimestamp(nowSec, nowNano + i * 4000));
    }

    // --- Create data columns for ingestion ---
    std::vector<DataColumn> columns = {
        makeDataColumn("ADC", adcData),
        makeDataColumn("Timestamps", timestampData)
    };

    // --- Create metadata for this data batch ---
    auto metadata = makeEventMetadata("example event", nowSec, nowNano, nowSec + 1, nowNano + 1000);
    
    // --- Build complete ingestion request ---
    auto request = makeIngestDataRequest(
         providerId,
         "0002",          // Client Request ID
         {},              // Attributes
         {},              // Tags
         metadata,
         clock,
         columns
    );

    // --- Send ingestion request to server ---
    client.ingestData(request);

    return 0;
}

