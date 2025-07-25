#include "udf/udf_types.h"

namespace udf {

int Column::get_index() const { return index; }
const std::string& Column::get_name() const { return name; }
const std::string& Column::get_type_kind() const { return type_kind; }

int Function::get_index() const { return index; }
const std::string& Function::get_name() const { return name; }
const std::string& Function::get_kind() const { return kind; }

const std::string& Service::get_name() const { return name; }

} // namespace udf
