#ifndef CPP_TALOGGERCONSUMER_H
#define CPP_TALOGGERCONSUMER_H

#include "TAConsumer.h"

namespace TaSDK {

    using namespace std;

    class LoggerConsumer : public TAConsumer {

    public:
        // 日志切分模式
        enum RotateMode {
            DAILY,             // 以天为单元
            HOURLY             // 以小时为单元
        };

    private:
        string fileName;                   // 文件名称
        int bufferSize;                     // messageBuffer 写入文件中的大小阈值, 即执行flush;
        int fileSize;                       // 单个文件大小
        string messageBuffer;                  // 缓存每条事件数据到内存中
        RotateMode rotateMode;

    public:
        class Config {

        public:
            string logDirectory;                           // 数据入库目录
            int bufferSize;                             // 内存缓存大小
            RotateMode rotateMode;                 // 默认以天为单元对日志进行分割
            string fileNamePrefix;                         // 日志文件前缀
            int fileSize;                             // 单个文件的大小阈值，默认值 0 代表 无穷大, 单位M

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