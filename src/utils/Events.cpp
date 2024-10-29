//
// Created by kido on 7/14/24.
//

#include "Events.hpp"

namespace vz {
Signal<int, int> Events::resizeSignal;
Signal<int> Events::keyPressedSignal;
Signal<int> Events::keyReleasedSignal;
Signal<int> Events::keyJustPressedSignal;
Signal<int> Events::mousePressedSignal;
Signal<int> Events::mouseReleasedSignal;
Signal<int> Events::mouseJustPressedSignal;
Signal<int,int> Events::mouseMovedSignal;
}