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
    // Signal if the key is pressed
    static Signal<int> keyPressedSignal;
    // Signal if the key is released
    static Signal<int> keyReleasedSignal;
    // Signal if the key is just pressed. Also, if the key is still pressed it does not send any further signal until it was released and pressed again.
    static Signal<int> keyJustPressedSignal;
    static Signal<int> mousePressedSignal;
    static Signal<int> mouseReleasedSignal;
    static Signal<int> mouseJustPressedSignal;
    static Signal<int,int> mouseMovedSignal;
};
}

#endif //EVENTS_HPP
