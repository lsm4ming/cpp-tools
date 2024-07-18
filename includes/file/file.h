#pragma once

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <functional>
#include "common/common.h"

constexpr int OPEN_FLAG = O_RDWR | O_APPEND | O_CLOEXEC | O_NONBLOCK;
constexpr int READ_FLAG = O_RDONLY | O_CLOEXEC | O_NONBLOCK;
constexpr int WRITE_FLAG = O_WRONLY | O_CLOEXEC | O_NONBLOCK;

using namespace cpptools::common;

namespace cpptools::file
{
    /**
     * input streams
     */
    class Reader
    {
    public:
        virtual ~Reader() = 0;

        /**
         * read data
         * @param data 目标缓冲区
         * @param length 读取长度
         * @return 读取长度
         */
        virtual int read(char *data, uint32 length) = 0;
    };

    /**
     * output streams
     */
    class Writer
    {
    public:
        virtual ~Writer() = 0;

        /**
         * write data
         * @param data 数据
         * @param length 数据长度
         * @return 写入长度
         */
        virtual int write(const char *data, uint32 length) = 0;
    };

    class File
    {
    private:
        String _pathname;

    public:
        explicit File(const String &pathname) : _pathname(pathname){};

        explicit File(String &&pathname, const int &mode = OPEN_FLAG) : _pathname(std::move(pathname)){};

        File(File &&other) noexcept : _pathname(std::move(other._pathname)){};

        ~File();

        /**
         * 文件是否存在
         * @return 是否存在
         */
        bool exists();

        /**
         * 判断是否为文件
         * @return 是否
         */
        bool isFile();

        /**
         * 判断是否为目录
         * @return 是否
         */
        bool isDir();

        /**
         * 判断是否为隐藏
         * @return 是否
         */
        bool isHidden();

        /**
         * 获取文件最后修改时间
         * @return 最后修改时间
         */
        int64 lastModified();

        /**
         * 获取文件长度
         * @return 文件长度
         */
        int64 length();

        /**
         * 创建目录
         * @return 是否创建成功
         */
        bool mkdir(int mode = 0777);

        /**
         * 递归创建目录
         * @return 是否创建成功
         */
        bool mkdirs(int mode = 0777);

        /**
         * 删除目录
         * @return 是否删除成功
         */
        bool rmdir();

        /**
         * 删除文件
         * @return 是否删除成功
         */
        bool deleteFile();

        /**
         * 创建文件
         * @return 是否创建成功
         */
        bool createNewFile();

    private:
        template <typename R>
        static R statOperation(const String &path, std::function<R(const struct stat &)> operation)
        {
            struct stat buffer;
            if (stat(path.c_str(), &buffer) == 0)
            {
                return operation(buffer);
            }
            return R();
        }
    };
}
