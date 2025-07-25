#pragma once
#include "plugin_api.h"
#include <string_view>
#include <vector>
namespace udf {
class plugin_loader {
  public:
    virtual ~plugin_loader()                                      = default;
    virtual void load(std::string_view dir_path)                  = 0;
    virtual void unload_all()                                     = 0;
    virtual const std::vector<plugin_api*>& apis() const noexcept = 0;
};
} // namespace udf
