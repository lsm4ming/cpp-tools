#include <iostream>
#include "cpptools/http/httpclient.h"

int main()
{
    using namespace cpptools::http;

    HttpClient client;
    client.setTimeout(3);
    auto result = client.get("http://192.168.31.177:9200/_cat/indices?v&pretty");
    auto text = result.getBody();
    std::cout << text << std::endl;
    return 0;
}