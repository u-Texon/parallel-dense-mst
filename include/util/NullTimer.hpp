#pragma once

#include "timer.hpp"


class NullTimer : public hybridMST::Timer {
    using Key = hybridMST::StandardKey;


public:
    NullTimer() = default;

    void start(const typename Key::Id &key_id, const typename Key::Count &count) {

    };

    void stop(const typename Key::Id &key_id, const typename Key::Count &count) {

    };

};
