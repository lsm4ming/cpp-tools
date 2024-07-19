#include "io/reader.h"

namespace cpptools::io
{
    FileReader::~FileReader()
    {
        this->close();
    }

    int FileReader::read(char *data, uint32 length)
    {
        this->_file.read(data, length);
        return this->_file.gcount();
    }

    bool FileReader::eof()
    {
        return this->_file.eof();
    }

    String FileReader::readLine()
    {
        std::string line;
        std::getline(this->_file, line);
        return line;
    }

    void FileReader::close()
    {
        this->_file.close();
    }
}
