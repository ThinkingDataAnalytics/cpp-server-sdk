#include "TDLoggerConsumer.h"
#include "TDUtils.h"
#include <sys/stat.h>

#include <utility>

namespace thinkingDataAnalytics {

    TDLoggerConsumer::TDLoggerConsumer(const Config& config) :
        m_namePrefix(config.fileNamePrefix),
        m_logDirectory(config.logDirectory),
        m_rotateMode(config.rotateMode),
        m_bufferSize(config.bufferSize),
        m_fileSize(config.fileSize),
        m_messageCount(0)
    {
        if (config.logDirectory.empty()) {
            ErrorLog("The specified directory path logDirectory cannot be empty!")
            return;
        }

#ifdef WIN32
        if (_access_s(m_logDirectory.c_str(), 0) == -1) {
            int32_t flag = _mkdir(m_logDirectory.c_str());
#else
        if (access(m_logDirectory.c_str(), 0) == -1) {
            int32_t flag = mkdir(m_logDirectory.c_str(), S_IRWXU);
#endif
            if (flag == 0) {
                std::cout << "Create directory successfully." << std::endl;
            }
            else {
                std::cout << "Fail to create directory." << std::endl;
                throw std::exception();
            }
        }

        TDLog::info("TDLoggerConsumer initialization successful");
    }

    void TDLoggerConsumer::flush() {
        TDLog::info("TDLoggerConsumer flush data.");
        m_flush_mutex.lock();
        sendData();
        m_flush_mutex.unlock();
    }

    void TDLoggerConsumer::add(const std::string& record) {
        if (record.empty()) {
            ErrorLog("Failed to add data, data is empty.")
            return;
        }

        m_flush_mutex.lock();
        
        m_messageBuffer += record;
        m_messageBuffer += "\n";
        m_messageCount++;

        TDLog::info("Enqueue data to buffer. Buffer count: " + std::to_string(m_messageCount) + ", data: " + record);

        // When there is data to upload, when the number of data cache reaches the bufferSize, upload the data immediately
        if (m_messageCount >= m_bufferSize) {
            sendData();
        }
        
        m_flush_mutex.unlock();
    }

    void TDLoggerConsumer::close() {
        m_flush_mutex.lock();
        sendData();
        m_outFile.flush();
        m_outFile.close();
        m_flush_mutex.unlock();

        TDLog::info("TDLoggerConsumer close.");
    }

    void TDLoggerConsumer::sendData() {
        if (m_messageBuffer.empty()) {
            return;
        }

        TDLog::info("TDLoggerConsumer write to file.");

        reloadOStream();

        if (m_outFile) {
            m_outFile.write(m_messageBuffer.c_str(), (std::streamsize)(strlen(m_messageBuffer.c_str())));
            m_outFile.flush();
        }
        else {
            ErrorLog("File open fail.")
        }
        m_messageBuffer = "";
        m_messageCount = 0;
    }

    std::string TDLoggerConsumer::generateRotateFileName() {
        std::string rotateFileName;

        if (!m_namePrefix.empty()) {
            rotateFileName = m_logDirectory + kPathSeparator + m_namePrefix + "log.";
        }
        else {
            rotateFileName = m_logDirectory + kPathSeparator + "log.";
        }

        time_t time_seconds = time(nullptr);
        struct tm ltm{};

#if defined(_WIN32)
        localtime_s(&ltm, &time_seconds);
#else
        localtime_r(&time_seconds, &ltm);
#endif
        rotateFileName += std::to_string((1900 + ltm.tm_year));
        rotateFileName += "-";
        rotateFileName += std::to_string((1 + ltm.tm_mon));
        rotateFileName += "-";
        rotateFileName += std::to_string(ltm.tm_mday);
        if (m_rotateMode == HOURLY) {
            rotateFileName += "-";
            rotateFileName += std::to_string(ltm.tm_hour);
        }

        return rotateFileName;
    }

    /// <summary>
    /// get file size. unit: byte
    /// </summary>
    /// <param name="fileName">: file name</param>
    /// <returns>file size. unit:byte </returns>
    size_t getFileSize(const char* fileName) {
        if (fileName == nullptr) {
            return 0;
        }
        struct stat statBuf{};
        stat(fileName, &statBuf);
        // byte
        size_t filesize = statBuf.st_size;
        return filesize;
    }

    void TDLoggerConsumer::reloadOStream() {
        if (m_fileSize <= 0) {
            std::cout << "file size is empty" << std::endl;
            return;
        }

        std::string newRotateName = generateRotateFileName();

        if (newRotateName == m_rotateFileName)
        {
            // Time has no changed

            if (getFileSize(m_currentFileName.c_str()) <= m_fileSize * 1024 * 1024)
            {
                return;
            }
            else
            {
                int32_t count = 0;
                std::string fullFileName = m_rotateFileName + "_" + std::to_string(count);
                std::ifstream f(fullFileName);
                while (f.good()) {
                    f.close();
                    ++count;
                    fullFileName = m_rotateFileName + "_" + std::to_string(count);
                    f = std::move(std::ifstream(fullFileName));
                }
                f.close();

                m_currentFileName = fullFileName;
                m_outFile = std::move(std::ofstream(fullFileName));
            }
        }
        else
        {
            if (m_rotateFileName.empty())
            {
                // service start

                m_rotateFileName = newRotateName;

                int32_t count = 0;
                std::string fullFileName = m_rotateFileName + "_" + std::to_string(count);
                std::ifstream f(fullFileName);
                while (f.good()) {
                    f.close();
                    ++count;
                    fullFileName = m_rotateFileName + "_" + std::to_string(count);
                    f = std::move(std::ifstream(fullFileName));
                }
                f.close();

                m_currentFileName = fullFileName;
                m_outFile = std::move(std::ofstream(fullFileName));
            }
            else
            {            
                // Time has changed. Split logs by time

                m_rotateFileName = newRotateName;

                int32_t count = 0;
                std::string fullFileName = m_rotateFileName + "_" + std::to_string(count);

                m_currentFileName = fullFileName;
                m_outFile = std::move(std::ofstream(fullFileName));
            }
        }
    }

    TDLoggerConsumer::Config::Config(std::string logDir, const int32_t bufferSize, const int32_t fileSize, const RotateMode rotateMode) :
        logDirectory(std::move(logDir)),
        bufferSize(bufferSize),
        fileSize(fileSize),
        rotateMode(rotateMode)
    {
    }
}