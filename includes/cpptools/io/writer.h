#pragma once

#include <fstream>
#include "file.h"

namespace cpptools::io
{
    class OutputStream : public IoStream
    {
        /**
         * 写入数据
         * @param data 缓冲区
         * @param length 数据长度
         * @return 写入长度
         */
        virtual int write(const char *data, uint32 length) = 0;
    };

    /**
     * output streams
     */
    class Writer : public OutputStream
    {
    };

    class FileWriter : public Writer
    {
    private:
        std::ofstream _file;

    public:
        explicit FileWriter(const String &filename, std::ios_base::openmode mode = std::ios_base::out) : _file(filename, mode){};

        explicit FileWriter(const File &file) : _file(file.getAbsolutePath()){};

        ~FileWriter();

        int write(const char *data, uint32 length) override;

        void close() override;
    };
}