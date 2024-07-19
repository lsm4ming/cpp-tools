#include "io/writer.h"

namespace cpptools::io
{
    FileWriter::~FileWriter()
    {
        this->close();
    }

    int FileWriter::write(const char *data, uint32 length)
    {
        std::streampos before = this->_file.tellp();
        this->_file.write(data, length);
        if (this->_file.fail())  // 写入失败
        {
            return -1;
        }
        return this->_file.tellp() - before;
    }

    void FileWriter::close()
    {
        this->_file.close();
    }
}
