#include "udf/udf_loader.h"
#include <dlfcn.h>
#include <filesystem>
#include <iostream>

namespace udf {
namespace fs = std::filesystem;

void udf_loader::load(std::string_view plugin_path) {
    fs::path path(plugin_path);
    if (fs::is_regular_file(path)) {
        if (path.extension() == ".so") {
            std::string full_path = path.string();
            void* handle          = dlopen(full_path.c_str(), RTLD_LAZY);
            if (!handle) {
                std::cerr << "Failed to load " << full_path << ": " << dlerror() << std::endl;
            } else {
                handles_.emplace_back(handle);
                create_api_from_handle(handle);
            }
        } else {
            std::cerr << "Specified file is not a .so plugin: " << path << std::endl;
        }
    } else if (fs::is_directory(path)) {
        for (const auto& entry : fs::directory_iterator(path)) {
            if (!entry.is_regular_file()) continue;
            if (entry.path().extension() != ".so") continue;

            std::string full_path = entry.path().string();
            void* handle          = dlopen(full_path.c_str(), RTLD_LAZY);
            if (!handle) {
                std::cerr << "Failed to load " << full_path << ": " << dlerror() << std::endl;
            } else {
                std::cout << "Loaded plugin: " << full_path << std::endl;
                handles_.emplace_back(handle);
                create_api_from_handle(handle);
            }
        }
    } else {
        std::cerr << "Plugin path is not valid: " << path << std::endl;
    }
}

void udf_loader::unload_all() {
    for (void* handle : handles_) {
        if (handle) dlclose(handle);
    }
    handles_.clear();
}
void udf_loader::create_api_from_handle(void* handle) {
    if (!handle) return;

    using create_func_type = plugin_api* (*)();
    auto* create_func      = reinterpret_cast<create_func_type>(dlsym(handle, "create_plugin_api"));

    if (!create_func) {
        std::cerr << "  Failed to find symbol create_plugin_api\n";
        return;
    }

    plugin_api* api = create_func();
    if (!api) {
        std::cerr << "  create_plugin_api returned nullptr\n";
        return;
    }
    apis_.emplace_back(api);
}
const std::vector<plugin_api*>& udf_loader::apis() const noexcept { return apis_; }

udf_loader::~udf_loader() { unload_all(); }

} // namespace udf
