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
        string fileName;                    // file name
        int32_t bufferSize;                     // When there is data to upload, when the number of data cache reaches the bufferSize, upload the data immediately
        int32_t fileSize;                       // single file size
        string messageBuffer;               // Cache each event data into memory
        int32_t messageCount;                   // Cache event data count into memory
        RotateMode rotateMode;

    public:
        class Config {

        public:
        
            string logDirectory;                        // Data storage directory
            int32_t bufferSize;                          // When there is data to upload, when the number of data cache reaches the bufferSize, upload the data immediately
            RotateMode rotateMode;                      // By default, the log is divided into days
            string fileNamePrefix;                      // log file prefix
            int32_t fileSize;                               // The size threshold of a single file, the default value is 0 means infinity, the unit is M

        public:
            // When there is data to upload, when the number of data cache reaches the bufferSize, upload the data immediately, default 20
            Config(const string &logDir, const int32_t bufferSize = 20);
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