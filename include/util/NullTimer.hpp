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
    //NullTimer(NullTimer const &) = delete;
    //void operator=(NullTimer const &) = delete;


    void start(const typename Key::Id &key_id, const typename Key::Count &count) {
        std::cout << "null start" << std::endl;
    };

    void stop(const typename Key::Id &key_id, const typename Key::Count &count) {
        std::cout << "null stop" << std::endl;
    };

};