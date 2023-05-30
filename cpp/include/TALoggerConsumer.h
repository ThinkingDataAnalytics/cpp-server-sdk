#ifndef CPP_TD_LOGGER_CONSUMER_H
#define CPP_TD_LOGGER_CONSUMER_H

#include "TAConsumer.h"

#if defined(_WIN32)
#include <io.h>
#include <windows.h>
#include <direct.h>
#include <iostream>
#include <fstream>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#endif

#include <mutex>


namespace TaSDK {

    using namespace std;

    /// <summary>
    /// write data to file. use it with logBus
    /// </summary>
    class LoggerConsumer : public TAConsumer {

    public:
        /// <summary>
        /// file rotate model¡£by days or hours
        /// </summary>
        enum RotateMode {
            DAILY,
            HOURLY
        };

        class Config;

        explicit LoggerConsumer(const Config& config);

        string generateRotateFileName();

        void add(const string& record) override;

        void flush() override;

        void close() override;

        void reloadOStream();

    private:
        string m_namePrefix;
        // file path
        string m_logDirectory;
        // When there is data to upload, when the number of data cache reaches the bufferSize, upload the data immediately
        int32_t m_bufferSize;
        // single file size
        int32_t m_fileSize;
        // Cache each event data into memory
        string m_messageBuffer;
        // Cache event data count into memory
        int32_t m_messageCount;
        RotateMode m_rotateMode;
        mutex m_flush_mutex;

        ofstream m_outFile;

        // e.g. ta.log.2023-02-02-12_0
        string m_currentFileName;

        // e.g. ta.log.2023-02-02-12 without index.
        string m_rotateFileName;
        
        void sendData();

    public:
        class Config {

        public:
            // Data storage directory
            string logDirectory;
            // When there is data to upload, when the number of data cache reaches the bufferSize, upload the data immediately
            int32_t bufferSize;
            // By default, the log is divided into days
            RotateMode rotateMode;
            // log file prefix
            string fileNamePrefix;
            // The size threshold of a single file, the default value is 0 means infinity, the unit is M
            int32_t fileSize;

            // When there is data to upload, when the number of data cache reaches the bufferSize, upload the data immediately, default 20. fileSize default 2M
            explicit Config(string logDir, int32_t bufferSize = 20, int32_t fileSize = 10, RotateMode rotateMode = LoggerConsumer::RotateMode::DAILY);
        };
        
    };
}

#endif