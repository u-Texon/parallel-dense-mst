#pragma once

#include "timer.hpp"


class NullTimer : public hybridMST::Timer {
    using Key = hybridMST::StandardKey;


public:
    static NullTimer &getInstance() {
        static NullTimer instance;
        return instance;
    }

private:
    NullTimer() = default;

    NullTimer(NullTimer const &);
    void operator=(NullTimer const &);


public:

    void start(const typename Key::Id &key_id, const typename Key::Count &count) {

    };

    void stop(const typename Key::Id &key_id, const typename Key::Count &count) {

    };

};