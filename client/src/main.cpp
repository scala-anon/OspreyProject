//TODO fix timestamp logic
// Combined header
#include "PacketParser.h"
#include "common.pb.h"
#include "ingestion.pb.h"
#include "ingestion.grpc.pb.h"

#include <grpc/grpc.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

namespace dping = dp::service::ingestion;

// --- Timestamp Helper ---
Timestamp makeTimeStamp(uint64_t epoch, uint64_t nano) {
    Timestamp ts;
    ts.set_epochseconds(epoch);
    ts.set_nanoseconds(nano);
    return ts;
}

// --- Attribute Helper ---
Attribute makeAttribute(const std::string& name, const std::string& value) {
    Attribute attr;
    attr.set_name(name);
    attr.set_value(value);
    return attr;
}

// --- EventMetadata Helper ---
EventMetadata makeEventMetadata(const std::string& desc, uint64_t startEpoch, uint64_t startNano,
                                uint64_t endEpoch, uint64_t endNano) {
    EventMetadata metadata;
    metadata.set_description(desc);
    *metadata.mutable_starttimestamp() = makeTimeStamp(startEpoch, startNano);
    *metadata.mutable_stoptimestamp() = makeTimeStamp(endEpoch, endNano);
    return metadata;
}

// --- SamplingClock Helper ---
SamplingClock makeSamplingClock(uint64_t epoch, uint64_t nano, uint64_t periodNanos, uint32_t count) {
    SamplingClock clock;
    *clock.mutable_starttime() = makeTimeStamp(epoch, nano);
    clock.set_periodnanos(periodNanos);
    clock.set_count(count);
    return clock;
}

// --- DataValue Helpers ---
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

// --- DataColumn Helper ---
DataColumn makeDataColumn(const std::string& name, const std::vector<DataValue>& values) {
    DataColumn col;
    col.set_name(name);
    for (const auto& val : values) {
        *col.add_datavalues() = val;
    }
    return col;
}

// --- IngestionDataFrame Helper ---
dping::IngestDataRequest::IngestionDataFrame makeIngestionDataFrame(const SamplingClock& samplingClock,
                                          const std::vector<DataColumn>& dataColumns) {
    dping::IngestDataRequest::IngestionDataFrame frame;
    *frame.mutable_datatimestamps()->mutable_samplingclock() = samplingClock;
    for (const auto& col : dataColumns) {
        *frame.add_datacolumns() = col;
    }
    return frame;
}

// --- IngestDataRequest Helper ---
dping::IngestDataRequest makeIngestDataRequest(const std::string& providerId, const std::string& clientRequestId, const std::vector<Attribute>& attributes, const std::vector<std::string>& tags, const EventMetadata& metadata, const SamplingClock& samplingClock, const std::vector<DataColumn>& dataColumns) {
    dping::IngestDataRequest request;
    request.set_providerid(providerId);
    request.set_clientrequestid(clientRequestId);


    for (const auto& attr : attributes) {
        *request.add_attributes() = attr;
    }

    for (const auto& tag : tags) {
        request.add_tags(tag);
    }

    *request.mutable_eventmetadata() = metadata;
    *request.mutable_ingestiondataframe() = makeIngestionDataFrame(samplingClock, dataColumns);

    return request;
}
dping::RegisterProviderRequest makeRegisterProviderRequest(const std::string& providerName, const std::vector<Attribute>& attributes, uint64_t epoch, uint64_t nano){
    dping::RegisterProviderRequest request;
    request.set_providername(providerName);
   
    for(const auto& attr : attributes) {
        *request.add_attributes() = attr;
    }
    return request;
}

dping::RegisterProviderResponse::RegistrationResult makeRegistrationResult(const std::string& providerId){
    dping::RegisterProviderResponse::RegistrationResult result;
    result.set_providerid(providerId);
    return result;
}

dping::RegisterProviderResponse makeRegisterProviderResponseWithResult(uint64_t epoch, uint64_t nano, const dping::RegisterProviderResponse::RegistrationResult& result){
    dping::RegisterProviderResponse response;
    *response.mutable_responsetime() = makeTimeStamp(epoch, nano);
    *response.mutable_registrationresult() = result;
    return response;
}

class OspreyClient {
public:
    OspreyClient(const std::string& server_address)
        : stub_(dp::service::ingestion::DpIngestionService::NewStub(grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials()))) {}

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
    std::unique_ptr<dp::service::ingestion::DpIngestionService::Stub> stub_;
};

int main() {
    const size_t CHUNK_SIZE = 1000;
    std::string server_address = "localhost:50051";
    OspreyClient client(server_address);

    uint64_t nowEpochNano = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    uint64_t nowSec = nowEpochNano / 1'000'000'000;
    uint32_t nowNano = nowEpochNano % 1'000'000'000;
    
    auto regReq = makeRegisterProviderRequest("Nick", {}, nowSec, nowNano);
    dping::RegisterProviderResponse regResp;
    try {
        regResp = client.sendRegisterProvider(regReq);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    if (!regResp.has_registrationresult()) {
        std::cerr << "Registration failed: no registration result in response." << std::endl;
        return 1;
    }
    std::string providerId = regResp.registrationresult().providerid();

    PacketParser parser("data/mic1-8-CH17-20240511-121442.dat");
    parser.parseFile();
    const std::vector<int32_t>& adcValues = parser.getAdcValues();
   
    for (size_t start = 0; start < adcValues.size(); start += CHUNK_SIZE) {
        size_t end = std::min(start + CHUNK_SIZE, adcValues.size());
        
        std::vector<DataValue> adcData;
        std::vector<DataValue> timestampData;
        for(size_t i = start; i < end; ++i){
        adcData.push_back(makeDataValueWithSInt32(adcValues[i]));
        uint64_t sampleNano = nowEpochNano + i * 4000;
        uint64_t sampleSec = sampleNano / 1'000'000'000;
        uint32_t sampleRemNanos = sampleNano % 1'000'000'000;

        timestampData.push_back(makeDataValueWithTimestamp(sampleSec, sampleRemNanos));
        }
    
    std::vector<DataColumn> columns = {
        makeDataColumn("ADC", adcData),
        makeDataColumn("Timestamps", timestampData)
    };

    uint64_t startNano = nowEpochNano + start * 4000;
    uint64_t stopNano = nowEpochNano + end * 4000;

    uint64_t startSec = startNano / 1'000'000'000;
    uint32_t startRemNanos = startNano % 1'000'000'000;

    uint64_t stopSec = stopNano / 1'000'000'000;
    uint32_t stopRemNanos = stopNano % 1'000'000'000;

    SamplingClock clock = makeSamplingClock(startSec, startRemNanos, 4000, end - start);
    EventMetadata metadata = makeEventMetadata("chunked upload", startSec, startRemNanos, stopSec, stopRemNanos);


    auto request = makeIngestDataRequest(
        providerId,
        "0002",
        {},
        {},
        metadata,
        clock,
        columns
    );

    client.ingestData(request);
    }
    return 0;
}

