#include <iostream>
#include "cpptools/time/time.h"
#include "cpptools/json/jsonparse.h"
#include "cpptools/http/httpclient.h"
#include "cpptools/http/httpserver.h"
#include "cpptools/log/log.h"

class myInterceptor : public cpptools::http::HttpInterceptor
{
    void interceptHandler(cpptools::http::InterceptContext &context) const override
    {
        auto t = context.request->getQuery("name");
        cpptools::log::LOG_INFO("myInterceptor: %s", t.c_str());
    }

    [[nodiscard]] Vector<String> interceptPath() const override
    {
        return {"/*"};
    }

    [[nodiscard]] int order() const override
    {
        return 1;
    }
};

void pong(cpptools::http::Request &req, cpptools::http::HttpResponseWriter &resp)
{
    // cpptools::log::LOG_INFO("处理一次请求");
    auto t = req.getQuery("name");
    resp.addHeader("Content-Type", "application/json; charset=utf-8");
    auto result = cpptools::json::JsonValue();
    result["code"] = 200;
    result["message"] = "操作成功";
    result["data"] = "pong";
    resp.write(result.toString());
}

void httpServerTest()
{
    cpptools::log::LOG_LEVEL(cpptools::log::DEBUG);
    cpptools::http::HttpServer server("0.0.0.0", 10040);

    server.addInterceptor(new myInterceptor);

    server.addRoute(cpptools::http::HttpMethod::HTTP_GET, "/ping", pong);
    server.addRoute(cpptools::http::HttpMethod::HTTP_GET, "/a/b", pong);
    server.addRoute(cpptools::http::HttpMethod::HTTP_GET, "/",
                    [](cpptools::http::Request &req, cpptools::http::HttpResponseWriter &resp)
                    {
                        auto t = req.getQuery("name");
                        resp.addHeader("Content-Type", "text/plain");
                        auto result = "hello: " + t;
                        resp.write(result.data(), result.length() + 1);
                    });
    server.addRoute(cpptools::http::HttpMethod::HTTP_POST, "/",
                    [](const cpptools::http::Request &req, cpptools::http::HttpResponseWriter &resp)
                    {
                        auto t = req.getQuery("name");
                        resp.addHeader("Content-Type", "text/plain");
                        auto result = "hello: " + t;
                        resp.write(result.data(), result.length() + 1);
                    });
    server.start();
}

int main(int argc, char **argv)
{
    httpServerTest();
    return 0;
}
