#ifndef LOADER_HPP
#define LOADER_HPP
#include <string>
#include <unordered_map>



namespace vz {
template <typename T>
class Loader {
public:
    Loader() = default;
    virtual ~Loader() {
        for (auto [key,value] : m_data) {
            delete value;
        }
    }

    virtual bool load(const std::string& path) = 0;
    T* get(const std::string& path) {
        if(m_data.contains(path)) {
            return m_data[path];
        }
        return nullptr;
    }
protected:
    std::unordered_map<std::string,T*> m_data;

};
}
#endif
