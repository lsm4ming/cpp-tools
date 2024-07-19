#pragma once

#include "file.h"
#include <fstream>

namespace cpptools::io
{
    /**
     * io stream
     */
    class IoStream
    {
    public:
        /**
         * 关闭
         */
        virtual void close() = 0;
    };

    class InputStream : public IoStream
    {
    public:
        /**
         * 读取数据
         * @param data 目标缓冲区
         * @param length 读取长度
         * @return 读取长度
         */
        virtual int read(char *data, uint32 length) = 0;

        /**
         * 是否读取完毕
         * @return 是否读取完毕
         */
        virtual bool eof() = 0;
    };

    /**
     * input streams
     */
    class Reader : public InputStream
    {
    };

    /*
     * output streams
     */
    class BufferReader : public IoStream
    {
    public:
        /**
         * 读取一行
         * @return 读取的一行数据
         */
        virtual String readLine() = 0;
    };

    class FileReader : public Reader,
                       public BufferReader
    {
    private:
        std::ifstream _file;

    public:
        explicit FileReader(const String &filename) : _file(filename){};

        explicit FileReader(const File &file) : _file(file.getAbsolutePath()){};

        FileReader(const FileReader &) = delete;

        FileReader &operator=(const FileReader &) = delete;

        FileReader(FileReader &&) = delete;

        ~FileReader();

        int read(char *data, uint32 length) override;

        bool eof() override;

        String readLine() override;

        void close() override;
    };
}
