#pragma once

#include "cpptools/common/common.h"

namespace cpptools::tools::generator
{
    using namespace cpptools::common;

    enum OutCodeType
    {
        CPP,
    };

    enum SourceType
    {
        ProtoBuf,
    };

    class CodeGenerator
    {
    public:
        CodeGenerator() = default;

        ~CodeGenerator() = default;

        /**
         * 根据内容，生成代码
         * @param content
         * @param outCodeType
         * @param sourceType
         * @return
         */
        virtual String generateWithContent(const String& content, OutCodeType outCodeType, SourceType sourceType) = 0;

        /**
         * 根据路径，生成代码
         * @param path
         * @param outCodeType
         * @return
         */
        virtual String generateWithPath(const String& path, OutCodeType outCodeType) = 0;
    };
}