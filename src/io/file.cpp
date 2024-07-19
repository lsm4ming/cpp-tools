#include "io/file.h"

namespace cpptools::io
{
    File::~File()
    {
    }

    bool File::exists() const
    {
        return statOperation<bool>(this->_pathname, [](const struct stat &buffer)
                                   { return true; });
    }

    bool File::isFile() const
    {
        return statOperation<bool>(this->_pathname, [](const struct stat &buffer)
                                   { return S_ISREG(buffer.st_mode); });
    }

    bool File::isDir() const
    {
        return statOperation<bool>(this->_pathname, [](const struct stat &buffer)
                                   { return S_ISDIR(buffer.st_mode); });
    }

    bool File::isHidden() const
    {
        return statOperation<bool>(this->_pathname, [](const struct stat &buffer)
                                   { return buffer.st_mode & S_IROTH; });
    }

    int64 File::lastModified() const
    {
        return statOperation<int64>(this->_pathname, [](const struct stat &buffer)
                                    { return buffer.st_mtime; });
    }

    int64 File::length() const
    {
        return statOperation<int64>(this->_pathname, [](const struct stat &buffer)
                                    { return buffer.st_size; });
    }

    bool File::mkdir(int mode) const
    {
        if (this->exists())
        {
            return false;
        }
        return ::mkdir(this->_pathname.c_str(), mode) == 0;
    }

    bool File::mkdirs(int mode) const
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

    bool File::rmdir() const
    {
        return ::rmdir(this->_pathname.c_str()) == 0;
    }

    bool File::deleteFile() const
    {
        return ::unlink(this->_pathname.c_str()) == 0;
    }

    bool File::createNewFile() const
    {
        return ::open(this->_pathname.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666) != -1;
    }

    String File::getName() const
    {
        return this->_pathname.substr(this->_pathname.find_last_of('/') + 1);
    }

    String File::getParent() const
    {
        String absolutePath = this->getAbsolutePath();
        size_t pos = absolutePath.find_last_of('/');
        if (pos == std::string::npos)
        {
            return String();
        }
        return absolutePath.substr(0, pos);
    }

    String File::getAbsolutePath() const
    {
        // 获取绝对路径
        return std::filesystem::absolute(this->_pathname).string();
    }
}