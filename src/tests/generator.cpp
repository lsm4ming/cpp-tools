#include <iostream>
#include "cpptools/tools/generator/protobuf/parser.h"

int main()
{
    auto path = "test.proto";
    auto tokens = cpptools::tools::generator::protobuf::ProtobufParser::parse2Token(path);
    for (auto &token: tokens)
    {
        std::cout << std::get<0>(token)  <<  std::get<1>(token) << std::endl;
    }

    cpptools::tools::generator::protobuf::ServiceInfo serviceInfo;
    cpptools::tools::generator::protobuf::ProtobufParser::parse2ServiceInfo(tokens, serviceInfo);

    std::cout << "包名=" << serviceInfo.package_name_ << std::endl;
    std::cout << "服务名=" << serviceInfo.service_name_ << std::endl;
    return 0;
}