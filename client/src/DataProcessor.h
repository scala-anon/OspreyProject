#ifndef DATAPROCESSOR_H
#define DATAPROCESSOR_H

#include "FileReader.h"  // Include the Sample struct and ReadSignalFile function
#include "common.grpc.pb.h"  // Include your protobuf header for DataColumn and DataValue
#include "common.pb.h"
#include "ingestion.grpc.pb.h"
#include "ingestion.pb.h"
// Function to populate a DataColumn with the given samples
DataColumn PopulateDataColumnFromSamples(const std::vector<Sample>& samples);

#endif // DATAPROCESSOR_H
