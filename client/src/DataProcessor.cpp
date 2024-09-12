#include "DataProcessor.h"
#include <variant>

// Function to populate DataColumn from the vector of samples
DataColumn PopulateDataColumnFromSamples(const std::vector<Sample>& samples) {
    DataColumn dataColumn;

    for (const auto& sample : samples) {
        DataValue dataValue;

        // Use std::visit to handle different types in the variant
        std::visit([&](auto&& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, int>) {
                dataValue.set_name("int");
                dataValue.set_int_value(value);  // Assuming DataValue has an int field
            } else if constexpr (std::is_same_v<T, float>) {
                dataValue.set_name("float");
                dataValue.set_float_value(value);  // Assuming DataValue has a float field
            } else if constexpr (std::is_same_v<T, std::string>) {
                dataValue.set_name("string");
                dataValue.set_string_value(value);  // Assuming DataValue has a string field
            }
            // Add more type checks for other types
        }, sample.value);

        // Add the populated DataValue to the DataColumn
        *dataColumn.add_datavalues() = dataValue;
    }

    return dataColumn;
}
