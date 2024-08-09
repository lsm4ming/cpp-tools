#pragma once

#include "common/common.h"
#include "http_router_trie.h"
#include "server_request.h"
#include "response_writer.h"

namespace cpptools::http
{
    using RouteHandler = Function<void(const Request &, HttpResponseWriter &)>;

    class HttpRouter
    {
    private:
    private:
        HashMap<std::string, Node *> roots;
        HashMap<std::string, RouteHandler> handlers;

    private:
        static Vector<String> parsePattern(const String &pattern);

        static String join(const Vector<String> &parts, size_t start, const String &delimiter);
    public:
        void addRoute(const String &method, const String &path, const RouteHandler &handler);

        RouteHandler getHandler(const String &method, const String &pattern) const;

        std::pair<Node *, SortMap<String, String>> getRoute(const String &method, const String &path);
    };
}