#include "udf/udf_bridge.h"
#include "udf/plugin_api.h"
#include "udf/udf_loader.h"
#include <stdexcept>
namespace udf {
udf_bridge::udf_bridge() : loader_(std::make_unique<udf_loader>()) {}
udf_bridge::~udf_bridge() = default;

std::string to_kind_string(function_kind_type kind) {
    switch (kind) {
        case function_kind_type::Unary: return "Unary";
        case function_kind_type::ClientStreaming: return "ClientStreaming";
        case function_kind_type::ServerStreaming: return "ServerStreaming";
        case function_kind_type::BidirectionalStreaming: return "BidirectionalStreaming";
        default: return "Unknown";
    }
}
std::string to_type_string(type_kind_type kind) {
    switch (kind) {
        case type_kind_type::Boolean: return "Boolean";
        case type_kind_type::Int4: return "Int4";
        case type_kind_type::Int8: return "Int8";
        case type_kind_type::Decimal: return "Decimal";
        case type_kind_type::Float4: return "Float4";
        case type_kind_type::Float8: return "Float8";
        case type_kind_type::Bytes: return "Bytes";
        case type_kind_type::String: return "String";
        case type_kind_type::Timestamp: return "Timestamp";
        case type_kind_type::TimestampWithTimeZone: return "TimestampWithTimeZone";
        case type_kind_type::Blob: return "Blob";
        case type_kind_type::Clob: return "Clob";
        default: return "Unknown";
    }
}

void udf_bridge::load(const std::string& plugin_dir) const { loader_->load(plugin_dir); }

std::size_t udf_bridge::service_count() const {
    std::size_t count = 0;
    for (const auto& api : loader_->apis()) {
        count += api->services().size();
    }
    return count;
}

std::unique_ptr<Service> udf_bridge::get_service_at(std::size_t idx) const {
    std::size_t current = 0;
    for (const auto& api : loader_->apis()) {
        for (const auto* svc : api->services()) {
            if (current == idx) {
                return std::make_unique<Service>(Service{std::string(svc->service_name())});
            }
            ++current;
        }
    }
    throw std::out_of_range("service index out of range");
}

std::size_t udf_bridge::function_count(std::size_t service_idx) const {
    std::size_t current = 0;
    for (const auto& api : loader_->apis()) {
        for (const auto* svc : api->services()) {
            if (current == service_idx) { return svc->functions().size(); }
            ++current;
        }
    }
    throw std::out_of_range("service index out of range");
}

std::unique_ptr<Function> udf_bridge::get_function_at(
    std::size_t service_idx, std::size_t func_idx) const {
    std::size_t current = 0;
    for (const auto& api : loader_->apis()) {
        for (const auto* svc : api->services()) {
            if (current == service_idx) {
                const auto* fn = svc->functions().at(func_idx);
                return std::make_unique<Function>(Function{static_cast<int>(fn->function_index()),
                    std::string(fn->function_name()), to_kind_string(fn->function_kind())});
            }
            ++current;
        }
    }
    throw std::out_of_range("function index out of range");
}

std::size_t udf_bridge::input_column_count(std::size_t service_idx, std::size_t func_idx) const {
    return get_function_descriptor(service_idx, func_idx).input_record().columns().size();
}

std::size_t udf_bridge::output_column_count(std::size_t service_idx, std::size_t func_idx) const {
    return get_function_descriptor(service_idx, func_idx).output_record().columns().size();
}

std::unique_ptr<Column> udf_bridge::get_input_column_at(
    std::size_t service_idx, std::size_t func_idx, std::size_t col_idx) const {
    const auto& cols = get_function_descriptor(service_idx, func_idx).input_record().columns();
    const auto* col  = cols.at(col_idx);
    return std::make_unique<Column>(Column{static_cast<int>(col->index()),
        std::string(col->column_name()), to_type_string(col->type_kind())});
}

std::unique_ptr<Column> udf_bridge::get_output_column_at(
    std::size_t service_idx, std::size_t func_idx, std::size_t col_idx) const {
    const auto& cols = get_function_descriptor(service_idx, func_idx).output_record().columns();
    const auto* col  = cols.at(col_idx);
    return std::make_unique<Column>(Column{static_cast<int>(col->index()),
        std::string(col->column_name()), to_type_string(col->type_kind())});
}

std::unique_ptr<udf_bridge> new_udf_bridge() { return std::make_unique<udf_bridge>(); }

const function_descriptor& udf_bridge::get_function_descriptor(
    std::size_t service_idx, std::size_t func_idx) const {
    std::size_t current = 0;
    for (const auto& api : loader_->apis()) {
        for (const auto* svc : api->services()) {
            if (current == service_idx) { return *svc->functions().at(func_idx); }
            ++current;
        }
    }
    throw std::out_of_range("function descriptor not found");
}

std::unique_ptr<std::string> make_string(const char* ptr) {
    return std::make_unique<std::string>(ptr ? ptr : "");
}
} // namespace udf
