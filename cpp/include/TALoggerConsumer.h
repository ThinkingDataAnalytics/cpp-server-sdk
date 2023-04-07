#ifndef CPP_TALOGGERCONSUMER_H
#define CPP_TALOGGERCONSUMER_H

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
    /// write data to file. use it with logbus
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

    private:
        string m_namePrefix;
        // file name
        string m_filePath;
        // file name
        string m_fileName;
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
        string m_currentFileNamePath;
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

        public:
            // When there is data to upload, when the number of data cache reaches the bufferSize, upload the data immediately, default 20. fileSize default 2M
            Config(const string& logDir, const int32_t bufferSize = 20, const int32_t fileSize = 10, const RotateMode rotateMode = LoggerConsumer::RotateMode::DAILY);
        };

    public:
        LoggerConsumer(const Config& config);

        void updateFilePath();

        void add(const string& record);

        void flush();

        void close();

        string getFileName();
    };
}

#endif