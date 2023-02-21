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




namespace TaSDK {

    using namespace std;

    LoggerConsumer::LoggerConsumer(const Config &config) {
        if (config.logDirectory.size() <= 0) {
            ErrorLog("The specified directory path logDirectory cannot be empty!");
            return;
        }
        fileName = config.logDirectory;

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

        if (config.fileNamePrefix.size()) {
            fileName = fileName + kPathSeparator + config.fileNamePrefix + "log.";
        } else {
            fileName = fileName + kPathSeparator + "log.";
        }

        time_t now = time(0);
        tm *ltm = localtime(&now);
        fileName += to_string((1900 + ltm->tm_year));
        fileName += "-";
        fileName += to_string((1 + ltm->tm_mon));
        fileName += "-";
        fileName += to_string(ltm->tm_mday);

        if (config.rotateMode == HOURLY) {
            fileName += "-";
            fileName += to_string(ltm->tm_hour);
        }

        rotateMode = config.rotateMode;
        bufferSize = config.bufferSize;
        fileSize = config.fileSize;
        messageCount = 0;

        cout << "LoggerConsumer Initialization successful" << endl;
    }

    LoggerConsumer::Config::Config(const string &logDir, const int bufferSize) : logDirectory(logDir),
                                                                              bufferSize(bufferSize) {
        cout << "Config Initialization successful" << endl;
    }

    void LoggerConsumer::add(const string &record) {

        if (record.size() <= 0) {
            ErrorLog("Failed to add data, data is empty.");
        }
        messageBuffer += record;
        messageBuffer += "\n";
        messageCount++;

        // When there is data to upload, when the number of data cache reaches the bufferSize, upload the data immediately
        if (messageCount >= bufferSize) {
            flush();
        }
    }

    string LoggerConsumer::getFileName() {
        // TA_LOCK(&ta_mutex);
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

        // TA_LOCK(&ta_mutex);
        if (messageBuffer.size() <= 0) return;

        string fileNamePath = getFileName();

        if (currentFileNamePath != fileNamePath) {
            outFile.close();
            outFile.open(fileNamePath, ios::out | ios::app);
            currentFileNamePath = fileNamePath;
        }

        if (outFile) {
            if (!(outFile << messageBuffer).bad()) {
                messageBuffer = "";
            } else {
                ErrorLog("Data write fail.");
            }
        } else {
            ErrorLog("File open fail.");
        }
        messageCount = 0;
        // TA_UNLOCK(&ta_mutex);

    }

    void LoggerConsumer::close() {
        flush();
        outFile.close();
    }
}