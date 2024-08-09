#include "http/request_parse.h"

namespace cpptools::http
{
    int RequestParse::parse(Request &request)
    {
        // 解析请求行
        if (this->parseRequestLine(request) < 0)
        {
            return -1;
        }
        // 解析请求头
        if (this->parseRequestHeader(request) < 0)
        {
            return -1;
        }
        return 0;
    }

    int RequestParse::parseRequestLine(Request &request)
    {
        int lineIndex = cpptools::utils::indexOf(this->raw, "\r\n");
        if (lineIndex == -1)
        {
            return -1;
        }
        String line = this->raw.substr(0, lineIndex);
        this->raw = this->raw.substr(lineIndex + 2);
        // 根据空格分隔字符串
        StringList line_match_result = cpptools::utils::split(line, EMPTY);
        if (line_match_result.size() != 3)
        {
            return -1;
        }
        request.method = stringToMethod(line_match_result[0]);
        request.url = line_match_result[1];
        request.version = line_match_result[2];

        // 解析query参数
        if (request.url.find(QUESTION_MARK) != String::npos)
        {
            Header query;
            request.queryRow = request.url.substr(request.url.find(QUESTION_MARK) + 1);
            request.url = request.url.substr(0, request.url.find(QUESTION_MARK));
            StringList querys = cpptools::utils::split(request.queryRow, AMP);
            for (auto &item: querys)
            {
                size_t index = item.find(EQUAL);
                if (index == String::npos)
                {
                    return -1;
                }
                String key = item.substr(0, index);
                String value = item.substr(index + 1);
                query[key].push_back(value);
            }
            request.setQuery(query);
        }
        return 0;
    }

    int RequestParse::parseRequestHeader(Request &request)
    {
        // 根据换行分隔字符串
        StringList line_match_result = cpptools::utils::split(this->raw, WRAP);
        Header header;
        for (auto &item: line_match_result)
        {
            // 根据冒号分隔字符串
            size_t index = item.find(COLON);
            if (index == String::npos)
            {
                return -1;
            }
            String key = item.substr(0, index);
            String value = item.substr(index + 1);
            header[key].push_back(value);
        }
        request.setHeader(header);
        return 0;
    }
}