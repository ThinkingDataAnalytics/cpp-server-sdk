
#ifndef CPP_TD_CONSUMER_H
#define CPP_TD_CONSUMER_H

#include <ostream>

namespace thinkingDataAnalytics {
    class TDConsumer {

    public:
        virtual ~TDConsumer() = 0;

        virtual void add(const std::string &record) = 0;

        virtual void flush() = 0;

        virtual void close() = 0;
    };
}

#endif