#pragma once
#include "plugin_api.h"
#include "plugin_loader.h"
#include <string_view>
#include <vector>
namespace udf {
class udf_loader : public plugin_loader {
  public:
    void load(std::string_view plugin_path) override;
    void unload_all() override;
    const std::vector<plugin_api*>& apis() const noexcept override;
    ~udf_loader() override;

  private:
    std::vector<void*> handles_;
    std::vector<plugin_api*> apis_;
    void create_api_from_handle(void* handle);
};

} // namespace udf