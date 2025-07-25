#pragma once
#include "udf_loader.h"
#include "udf_types.h"
#include <memory>
#include <string>

namespace udf {

std::unique_ptr<std::string> make_string(const char* ptr);

class udf_bridge {
  public:
    udf_bridge();
    ~udf_bridge();

    void load(const std::string& plugin_dir) const;

    std::size_t service_count() const;

    std::unique_ptr<Service> get_service_at(std::size_t idx) const;

    std::size_t function_count(std::size_t service_idx) const;
    std::unique_ptr<Function> get_function_at(std::size_t service_idx, std::size_t func_idx) const;

    std::size_t input_column_count(std::size_t service_idx, std::size_t func_idx) const;
    std::unique_ptr<Column> get_input_column_at(
        std::size_t service_idx, std::size_t func_idx, std::size_t col_idx) const;

    std::size_t output_column_count(std::size_t service_idx, std::size_t func_idx) const;
    std::unique_ptr<Column> get_output_column_at(
        std::size_t service_idx, std::size_t func_idx, std::size_t col_idx) const;

  private:
    std::unique_ptr<udf_loader> loader_;
    const function_descriptor& get_function_descriptor(std::size_t sidx, std::size_t fidx) const;
};

std::unique_ptr<udf_bridge> new_udf_bridge();
} // namespace udf