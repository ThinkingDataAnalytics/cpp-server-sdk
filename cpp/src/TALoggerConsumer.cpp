#include "../include/TALoggerConsumer.h"
#include "../include/TAUtils.h"

#if defined(_WIN32)
#include <windows.h>
#include <iostream>
#include <fstream>
#include <direct.h>
#endif

namespace TaSDK {

    using namespace std;

    LoggerConsumer::LoggerConsumer(const Config &config) {
        if (config.logDirectory.size() <= 0) {
            ErrorLog("指定的目录路径logDirectory不能为空！");
            return;
        }
        fileName = config.logDirectory;

        // 判断文件夹是否存在
        if (access(fileName.c_str(), F_OK) == -1) {
#ifdef WIN32
            int flag = mkdir(fileName.c_str());  //Windows创建文件夹
#else
            int flag = mkdir(fileName.c_str(), S_IRWXU);  //Linux创建文件夹
#endif
            if (flag == 0) {  //创建成功
                std::cout << "Create directory successfully." << std::endl;
            } else { //创建失败
                std::cout << "Fail to create directory." << std::endl;
                throw std::exception();
            }
        }

        if (config.fileNamePrefix.size()) {
            fileName = fileName + kPathSeparator + config.fileNamePrefix + "log.";
        } else {
            fileName = fileName + kPathSeparator + "log.";
        }

        // 基于当前系统的当前日期/时间
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

        cout << "LoggerConsumer初始化成功" << endl;
    }

    LoggerConsumer::Config::Config(const string &logDir, const int bufferS) : logDirectory(logDir),
                                                                              bufferSize(bufferS) {
        cout << "Config初始化成功" << endl;
    }

    void LoggerConsumer::add(const string &record) {

        if (record.size() <= 0) {
            ErrorLog("Failed to add data, data is empty.");
        }
        messageBuffer += record;
        messageBuffer += "\n";

        if (sizeof(messageBuffer) >= bufferSize) {
            flush();
        }
    }

    string LoggerConsumer::getFileName() {
        // TA_LOCK(&ta_mutex);
        int count = 0;
        string fullFileName = fileName + "_" + to_string(count);
        if (fileSize > 0) {
            ifstream inFile(fullFileName, ios::in | ios::binary);
            while (inFile) {
                long l, m;
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

    ofstream outFile;                        // 打开文件
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
        // TA_UNLOCK(&ta_mutex);
    }

    void LoggerConsumer::close() {
        flush();
        outFile.close();
    }
}