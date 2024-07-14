#ifndef EVENTS_HPP
#define EVENTS_HPP
#include <functional>
#include <vector>

namespace vz {
template<typename... Args>
class Signal {
public:
    void connect(std::function<void(Args...)> slot) {
        slots.push_back(slot);
    }
    void emit(Args... args) {
        for (auto& slot : slots) {
            slot(args...);
        }
    }
private:
    std::vector<std::function<void(Args...)>> slots;
};

class Events {
public:
    // Signal if the windows resizes. Width,Height
    static Signal<int,int> resizeSignal;
};
}

#endif //EVENTS_HPP
