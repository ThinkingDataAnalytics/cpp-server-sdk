#include "TALoggerConsumer.h"
#include "TAUtils.h"

namespace TaSDK {
    using namespace std;

    LoggerConsumer::Config::Config(const string& logDir, const int32_t bufferSize, const int32_t fileSize, const RotateMode rotateMode) :
        logDirectory(logDir),
        bufferSize(bufferSize),
        fileSize(fileSize),
        rotateMode(rotateMode)
    {
        cout << "[ThinkingEngine] Config Initialization successful" << endl;
    }

    void LoggerConsumer::updateFilePath() {
        if (m_namePrefix.size()) {
            m_fileName = m_filePath + kPathSeparator + m_namePrefix + "log.";
        } else {
            m_fileName = m_filePath + kPathSeparator + "log.";
        }

        time_t time_seconds = time(0);
        struct tm ltm;

#if defined(_WIN32)
        localtime_s(&ltm, &time_seconds);
#else
        localtime_r(&time_seconds, &ltm);
#endif
        m_fileName += to_string((1900 + ltm.tm_year));
        m_fileName += "-";
        m_fileName += to_string((1 + ltm.tm_mon));
        m_fileName += "-";
        m_fileName += to_string(ltm.tm_mday);
        if (m_rotateMode == HOURLY) {
            m_fileName += "-";
            m_fileName += to_string(ltm.tm_hour);
        }
    }

    void LoggerConsumer::flush() {
        m_flush_mutex.lock();
        sendData();
        m_flush_mutex.unlock();
    }

    void LoggerConsumer::add(const string& record) {
        if (record.size() <= 0) {
            ErrorLog("Failed to add data, data is empty.");
            return;
        }

        m_flush_mutex.lock();
        
        m_messageBuffer += record;
        m_messageBuffer += "\n";
        m_messageCount++;

        // When there is data to upload, when the number of data cache reaches the bufferSize, upload the data immediately
        if (m_messageCount >= m_bufferSize) {
            sendData();
        }
        
        m_flush_mutex.unlock();
    }

    string LoggerConsumer::getFileName() {
        updateFilePath();
        int32_t count = 0;
        string fullFileName = m_fileName + "_" + to_string(count);
        if (m_fileSize > 0) {
            ifstream inFile(fullFileName, ios::in | ios::binary);
            while (inFile) {
                int64_t l, m;
                l = inFile.tellg();
                inFile.seekg(0, ios::end);
                m = inFile.tellg();
                if (((m - l) / (static_cast<long long>(1024) * 1024)) < m_fileSize) {
                    break;
                }
                ++count;
                fullFileName = m_fileName + "_" + to_string(count);
                inFile.close();
                inFile.open(fullFileName, ios::in | ios::binary);
            }
        }
        return fullFileName;
    }

    void LoggerConsumer::close() {
        m_flush_mutex.lock();
        sendData();
        m_outFile.flush();
        m_outFile.close();
        m_flush_mutex.unlock();
    }

    void LoggerConsumer::sendData() {
        if (m_messageBuffer.size() <= 0) {
            return;
        }

        string fileNamePath = getFileName();

        if (m_currentFileNamePath != fileNamePath) {
            m_outFile.close();
            m_outFile.open(fileNamePath, ios::out | ios::app);
            m_currentFileNamePath = fileNamePath;
        }

        if (m_outFile) {
            m_outFile.write(m_messageBuffer.c_str(), strlen(m_messageBuffer.c_str()));
            m_outFile.flush();
        }
        else {
            ErrorLog("File open fail.");
        }
        m_messageBuffer = "";
        m_messageCount = 0;
    }

    LoggerConsumer::LoggerConsumer(const Config& config) :
        m_fileName(config.logDirectory),
        m_namePrefix(config.fileNamePrefix),
        m_filePath(config.logDirectory),
        m_rotateMode(config.rotateMode),
        m_bufferSize(config.bufferSize),
        m_fileSize(config.fileSize),
        m_messageCount(0)
    {
        if (config.logDirectory.size() <= 0) {
            ErrorLog("The specified directory path logDirectory cannot be empty!");
            return;
        }

        int32_t flag = -1;

#ifdef WIN32
        if (_access_s(m_fileName.c_str(), 0) == -1) {
            int32_t flag = _mkdir(m_fileName.c_str());
#else
        if (access(m_fileName.c_str(), 0) == -1) {
            int32_t flag = mkdir(m_fileName.c_str(), S_IRWXU);
#endif
            if (flag == 0) {
                std::cout << "Create directory successfully." << std::endl;
            }
            else {
                std::cout << "Fail to create directory." << std::endl;
                throw std::exception();
            }
        }

        updateFilePath();

        cout << "[ThinkingEngine] LoggerConsumer Initialization successful" << endl;    
    }
}