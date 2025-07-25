#pragma once
#include <string>

namespace udf {
struct Column {
    int index;
    std::string name;
    std::string type_kind;

    int get_index() const;
    const std::string& get_name() const;
    const std::string& get_type_kind() const;
};

struct Function {
    int index;
    std::string name;
    std::string kind;

    int get_index() const;
    const std::string& get_name() const;
    const std::string& get_kind() const;
};

struct Service {
    std::string name;

    const std::string& get_name() const;
};
} // namespace udf