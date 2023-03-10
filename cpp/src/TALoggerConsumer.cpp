#include "TALoggerConsumer.h"
#include "TAUtils.h"

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

#include <pthread.h>
#include <mutex>


namespace TaSDK {

    using namespace std;

    pthread_mutex_t flush_mutex_t;

    LoggerConsumer::LoggerConsumer(const Config &config) {
        if (config.logDirectory.size() <= 0) {
            ErrorLog("The specified directory path logDirectory cannot be empty!");
            return;
        }
        fileName_origin = config.logDirectory;
        fileName = config.logDirectory;
        fileNamePrefix = config.fileNamePrefix;
        rotateMode = config.rotateMode;

        int32_t flag = -1;

#ifdef WIN32
        if (_access_s(fileName.c_str(), 0) == -1) {
            int32_t flag = mkdir(fileName.c_str());
#else
        if (access(fileName.c_str(), 0) == -1) {
            int32_t flag = mkdir(fileName.c_str(), S_IRWXU);
#endif
            if (flag == 0) {
                std::cout << "Create directory successfully." << std::endl;
            } else {
                std::cout << "Fail to create directory." << std::endl;
                throw std::exception();
            }
        }

        updateFilePath();

        rotateMode = config.rotateMode;
        bufferSize = config.bufferSize;
        fileSize = config.fileSize;
        messageCount = 0;

        pthread_mutex_init(&flush_mutex_t, 0);

        cout << "LoggerConsumer Initialization successful" << endl;
    }

   LoggerConsumer::Config::Config(const string &logDir, const int bufferSize, const int fileSize, const RotateMode rotateMode) : logDirectory(logDir),
                                                                                                     bufferSize(bufferSize), fileSize(fileSize), rotateMode(rotateMode) {
        cout << "Config Initialization successful" << endl;
    }

    void LoggerConsumer::add(const string &record) {

        int32_t _messageCount = 0;
        int32_t _bufferSize = 0;
        pthread_mutex_lock(&flush_mutex_t);

        if (record.size() <= 0) {
            ErrorLog("Failed to add data, data is empty.");
        }
        messageBuffer += record;
        messageBuffer += "\n";
        messageCount++;

        _messageCount = messageCount;
        _bufferSize = bufferSize;
        pthread_mutex_unlock(&flush_mutex_t);

        // When there is data to upload, when the number of data cache reaches the bufferSize, upload the data immediately
        if (_messageCount >= _bufferSize) {
            flush();
        }
    }

    string LoggerConsumer::getFileName() {
        // TA_LOCK(&ta_mutex);
        updateFilePath();
        int32_t count = 0;
        string fullFileName = fileName + "_" + to_string(count);
        if (fileSize > 0) {
            ifstream inFile(fullFileName, ios::in | ios::binary);
            while (inFile) {
                int64_t l, m;
                l = inFile.tellg();
                inFile.seekg(0, ios::end);
                m = inFile.tellg();
                if (((m - l) / (1024 * 1024)) < fileSize) {
                    break;
                }
                ++count;
                fullFileName = fileName + "_" + to_string(count);
                inFile.close();
                inFile.open(fullFileName, ios::in | ios::binary);
            }
        }
        // TA_UNLOCK(&ta_mutex);
        return fullFileName;
    }

    ofstream outFile;
    string currentFileNamePath;

    void LoggerConsumer::flush() {

        pthread_mutex_lock(&flush_mutex_t);
        if (messageBuffer.size() <= 0) {
            pthread_mutex_unlock(&flush_mutex_t);
            return;
        }

        string fileNamePath = getFileName();

        if (currentFileNamePath != fileNamePath) {
            outFile.close();
            outFile.open(fileNamePath, ios::out | ios::app);
            currentFileNamePath = fileNamePath;
        }

        if (outFile) {
            outFile.write(messageBuffer.c_str(), strlen(messageBuffer.c_str()));
            outFile.flush();
        } else {
            ErrorLog("File open fail.");
        }
        messageBuffer = "";
        messageCount = 0;
        
        pthread_mutex_unlock(&flush_mutex_t);
    }

    void LoggerConsumer::close() {
        flush();
        outFile.flush();
        outFile.close();
    }

    void LoggerConsumer::updateFilePath() {
        if (fileNamePrefix.size()) {
            fileName = fileName_origin + kPathSeparator + fileNamePrefix + "log.";
        } else {
            fileName = fileName_origin + kPathSeparator + "log.";
        }

        time_t now = time(0);
        tm *ltm = localtime(&now);
        fileName += to_string((1900 + ltm->tm_year));
        fileName += "-";
        fileName += to_string((1 + ltm->tm_mon));
        fileName += "-";
        fileName += to_string(ltm->tm_mday);

        if (rotateMode == HOURLY) {
            fileName += "-";
            fileName += to_string(ltm->tm_hour);
        }
    }
}