#include "file/file.h"

namespace cpptools::file
{
    File::~File()
    {
    }

    bool File::exists()
    {
        return statOperation<bool>(this->_pathname, [](const struct stat &buffer)
                                   { return true; });
    }

    bool File::isFile()
    {
        return statOperation<bool>(this->_pathname, [](const struct stat &buffer)
                                   { return S_ISREG(buffer.st_mode); });
    }

    bool File::isDir()
    {
        return statOperation<bool>(this->_pathname, [](const struct stat &buffer)
                                   { return S_ISDIR(buffer.st_mode); });
    }

    bool File::isHidden()
    {
        return statOperation<bool>(this->_pathname, [](const struct stat &buffer)
                                   { return buffer.st_mode & S_IROTH; });
    }

    int64 File::lastModified()
    {
        return statOperation<int64>(this->_pathname, [](const struct stat &buffer)
                                    { return buffer.st_mtime; });
    }

    int64 File::length()
    {
        return statOperation<int64>(this->_pathname, [](const struct stat &buffer)
                                    { return buffer.st_size; });
    }

    bool File::mkdir(int mode)
    {
        if (this->exists())
        {
            return false;
        }
        return ::mkdir(this->_pathname.c_str(), mode) == 0;
    }

    bool File::mkdirs(int mode)
    {
        size_t pos = 0;
        std::string subpath;

        while ((pos = this->_pathname.find_first_of('/', pos)) != std::string::npos)
        {
            subpath = this->_pathname.substr(0, pos++);
            if (subpath.empty())
            {
                continue;
            }
            struct stat st;
            if (stat(subpath.c_str(), &st) == -1)
            {
                if (::mkdir(subpath.c_str(), mode) != 0)
                {
                    return false;
                }
            }
            else if (!S_ISDIR(st.st_mode))
            {
                return false;
            }
        }

        if (::mkdir(this->_pathname.c_str(), mode) != 0)
        {
            return false;
        }
        return true;
    }

    bool File::rmdir()
    {
        return ::rmdir(this->_pathname.c_str()) == 0;
    }

    bool File::deleteFile()
    {
        return ::unlink(this->_pathname.c_str()) == 0;
    }

    bool File::createNewFile()
    {
        return ::open(this->_pathname.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666) != -1;
    }
}
