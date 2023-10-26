#ifndef CPP_TD_LOGGER_CONSUMER_H
#define CPP_TD_LOGGER_CONSUMER_H

#include "TDConsumer.h"

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


namespace thinkingDataAnalytics {

    /**
     * Write data to file. use it with logBus
     */
    class TDLoggerConsumer : public TDConsumer {

    public:
        /**
         * File rotate model by days or hours
         */
        enum RotateMode {
            DAILY,
            HOURLY
        };

        class Config;

        explicit TDLoggerConsumer(const Config& config);

        std::string generateRotateFileName();

        void add(const std::string& record) override;

        void flush() override;

        void close() override;

        void reloadOStream();

    private:
        std::string m_namePrefix;
        // file path
        std::string m_logDirectory;
        // When there is data to upload, when the number of data cache reaches the bufferSize, upload the data immediately
        int32_t m_bufferSize;
        // single file size
        int32_t m_fileSize;
        // Cache each event data into memory
        std::string m_messageBuffer;
        // Cache event data count into memory
        int32_t m_messageCount;
        RotateMode m_rotateMode;
        std::mutex m_flush_mutex;

        std::ofstream m_outFile;

        // e.g. ta.log.2023-02-02-12_0
        std::string m_currentFileName;

        // e.g. ta.log.2023-02-02-12 without index.
        std::string m_rotateFileName;
        
        void sendData();

    public:
        /**
         * LoggerConsumer config
         */
        class Config {

        public:
            // Data storage directory
            std::string logDirectory;
            // When there is data to upload, when the number of data cache reaches the bufferSize, upload the data immediately
            int32_t bufferSize;
            // By default, the log is divided into days
            RotateMode rotateMode;
            // log file prefix
            std::string fileNamePrefix;
            // The size threshold of a single file, the default value is 0 means infinity, the unit is M
            int32_t fileSize;

            // When there is data to upload, when the number of data cache reaches the bufferSize, upload the data immediately, default 20. fileSize default 2M
            explicit Config(std::string logDir, int32_t bufferSize = 20, int32_t fileSize = 10, RotateMode rotateMode = TDLoggerConsumer::RotateMode::DAILY);
        };
        
    };
}

#endif