#pragma once

#include <unistd.h>
#include <sys/stat.h>
#include <functional>
#include <filesystem>
#include "define.h"

using namespace cpptools::common;

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
  
    class File
    {
    private:
        String _pathname;

    public:
        explicit File(const String &pathname) : _pathname(pathname){};

        explicit File(String &&pathname, const int &mode = OPEN_FLAG) : _pathname(std::move(pathname)){};

        File(const File &other) : _pathname(other._pathname){};

        File(File &&other) noexcept : _pathname(std::move(other._pathname)){};

        File &operator=(const File &)
        {
            return *this;
        };

        ~File();

        /**
         * 文件是否存在
         * @return 是否存在
         */
        bool exists() const;

        /**
         * 判断是否为文件
         * @return 是否
         */
        bool isFile() const;

        /**
         * 判断是否为目录
         * @return 是否
         */
        bool isDir() const;

        /**
         * 判断是否为隐藏
         * @return 是否
         */
        bool isHidden() const;

        /**
         * 获取文件最后修改时间
         * @return 最后修改时间
         */
        int64 lastModified() const;

        /**
         * 获取文件长度
         * @return 文件长度
         */
        int64 length() const;

        /**
         * 创建目录
         * @return 是否创建成功
         */
        bool mkdir(int mode = 0777) const;

        /**
         * 递归创建目录
         * @return 是否创建成功
         */
        bool mkdirs(int mode = 0777) const;

        /**
         * 删除目录
         * @return 是否删除成功
         */
        bool rmdir() const;

        /**
         * 删除文件
         * @return 是否删除成功
         */
        bool deleteFile() const;

        /**
         * 创建文件
         * @return 是否创建成功
         */
        bool createNewFile() const;

        /**
         * 获取文件名
         * @return 文件名
         */
        String getName() const;

        /**
         * 获取父目录
         * @return 父目录
         */
        String getParent() const;

        /**
         * 获取绝对路径
         * @return 绝对路径
         */
        String getAbsolutePath() const;

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
