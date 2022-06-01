//
// Created by 沈和平 on 2022/5/17.
//

#ifndef CPP_TACONSUMER_H
#define CPP_TACONSUMER_H

#include <ostream>

namespace TaSDK {
    class TAConsumer {

    public:
        virtual void add(const std::string &record) = 0;

        virtual void flush() = 0;

        virtual void close() = 0;
    };
}

#endif //CPP_TACONSUMER_H
