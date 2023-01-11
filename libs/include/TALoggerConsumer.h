#ifndef CPP_TALOGGERCONSUMER_H
#define CPP_TALOGGERCONSUMER_H

#include "TAConsumer.h"

namespace TaSDK {

    using namespace std;

    class LoggerConsumer : public TAConsumer {

    public:
        enum RotateMode {
            DAILY,             // in days
            HOURLY             // in hours
        };

    private:
        string fileName;                   // file name
        int bufferSize;                     // messageBuffer writes the size threshold in the file
        int fileSize;                       // single file size
        string messageBuffer;                  // Cache each event data into memory
        RotateMode rotateMode;

    public:
        class Config {

        public:
            string logDirectory;                           // Data storage directory
            int bufferSize;                             // memory cache size
            RotateMode rotateMode;                 // By default, the log is divided into days
            string fileNamePrefix;                         // log file prefix
            int fileSize;                             // The size threshold of a single file, the default value is 0 means infinity, the unit is M

        public:
            Config(const string &logDir, const int bufferS = 8192);
        };

    public:
        LoggerConsumer(const Config &config);

        void add(const string &record);

        void flush();

        void close();

        string getFileName();
    };

}

#endif