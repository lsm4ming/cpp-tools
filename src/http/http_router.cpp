#include "http/http_router.h"

namespace cpptools::http
{
    void cpptools::http::HttpRouter::addRoute(const String &method, const String &path,
                                              const cpptools::http::RouteHandler &handler)
    {
        Vector<String> parts = parsePattern(path);
        String key = method + "-" + path;
        if (roots.find(method) == roots.end())
        {
            roots[method] = new Node("", false);
        }
        roots[method]->insert(path, parts, 0);
        handlers[key] = handler;
    }

    Vector<String> cpptools::http::HttpRouter::parsePattern(const String &pattern)
    {
        std::vector<String> parts;
        std::istringstream iss(pattern);
        String item;
        while (std::getline(iss, item, '/'))
        {
            if (!item.empty())
            {
                parts.push_back(item);
                if (item[0] == '*')
                {
                    break;
                }
            }
        }
        return parts;
    }

    String cpptools::http::HttpRouter::join(const Vector<String> &parts, size_t start, const String &delimiter)
    {
        String result;
        for (size_t i = start; i < parts.size(); i++)
        {
            result += parts[i];
            if (i != parts.size() - 1)
            {
                result += delimiter;
            }
        }
        return result;
    }

    std::pair<Node *, SortMap<String, String>>
    cpptools::http::HttpRouter::getRoute(const String &method, const String &path)
    {
        Vector<String> searchParts = parsePattern(path);
        SortMap<String, String> params;
        auto it = roots.find(method);
        if (it == roots.end())
        {
            return {nullptr, params};
        }
        Node *root = it->second;
        Node *n = root->search(searchParts, 0);
        if (n != nullptr)
        {
            Vector<String> parts = parsePattern(n->getPattern());
            for (size_t i = 0; i < parts.size(); i++)
            {
                if (parts[i][0] == ':')
                {
                    params[parts[i].substr(1)] = searchParts[i];
                }
                if (parts[i][0] == '*' && parts[i].length() > 1)
                {
                    params[parts[i].substr(1)] = join(searchParts, i, "/");
                    break;
                }
            }
            return {n, params};
        }
        return {nullptr, params};
    }

    RouteHandler HttpRouter::getHandler(const String &method, const String &pattern) const
    {
        auto it = handlers.find(method + '-' + pattern);
        if (it == handlers.end())
        {
            return nullptr;
        }
        return it->second;
    }
}
