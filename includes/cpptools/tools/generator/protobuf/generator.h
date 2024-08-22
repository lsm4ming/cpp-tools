#pragma once

#include "cpptools/common/common.h"
#include "../generator.h"

namespace cpptools::tools::generator::protobuf
{
    using namespace cpptools::common;

    class ProtobufGenerator : public CodeGenerator
    {
    public:
        ProtobufGenerator();
        ~ProtobufGenerator();

        String generateWithContent(const String &content, OutCodeType outCodeType, SourceType sourceType) override;

        String generateWithPath(const String &path, OutCodeType outCodeType) override;
    };
}