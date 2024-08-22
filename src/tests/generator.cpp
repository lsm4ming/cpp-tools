#include <iostream>
#include "cpptools/tools/generator/protobuf/parser.h"

int main()
{
    auto path = "test.proto";
    auto tokens = cpptools::tools::generator::protobuf::ProtobufParser::parse2Token(path);
    for (auto &token: tokens)
    {
        std::cout << std::get<cpptools::common::String>(token) << std::endl;
    }
    return 0;
}