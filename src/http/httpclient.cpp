#include "http/httpclient.h"

namespace cpptools::http
{
    HttpClient *HttpClient::setQuery(const String &key, const String &value)
    {
        this->request.setQuery(key, value);
        return this;
    }

    HttpClient *HttpClient::addQuery(const String &key, const String &value)
    {
        this->_request.addQuery(key, value);
        return this;
    }

    HttpClient *HttpClient::setQuery(const String &key, const Vector<String> &values)
    {
        this->_request.setQuery(key, values);
        return this;
    }

    HttpClient *HttpClient::addQuery(const String &key, const Vector<String> &values)
    {
        this->_request.addQuery(key, values);
        return this;
    }

    HttpClient *HttpClient::setHeader(const String &key, const String &value)
    {
        return this->setHeader(key, Vector<String>{value});
    }

    HttpClient *HttpClient::addHeader(const String &key, const String &value)
    {
        return this->addHeader(key, Vector<String>{value});
    }

    HttpClient *HttpClient::setHeader(const String &key, const Vector<String> &values)
    {
        this->_request.setHeader(key, values);
        return this;
    }

    HttpClient *HttpClient::addHeader(const String &key, const Vector<String> &values)
    {
        this->_request.addHeader(key, values);
        return this;
    }

    HttpClient *HttpClient::setBody(char *body, size_t length)
    {
        this->_request.setBody(body, length);
        return this;
    }

    HttpClient *HttpClient::setTimeout(int seconds)
    {
        this->_request.setTimeout(seconds);
        return this;
    }

    HttpResponse HttpClient::post(const String &url)
    {
        return this->doSend(HttpMethod::HTTP_POST, url);
    }

    HttpResponse HttpClient::get(const String &url)
    {
        return this->doSend(HttpMethod::HTTP_GET, url);
    }

    HttpResponse HttpClient::put(const String &url)
    {
        return this->doSend(HttpMethod::HTTP_PUT, url);
    }

    HttpResponse HttpClient::delete_(const String &url)
    {
        return this->doSend(HttpMethod::HTTP_DELETE, url);
    }

    HttpResponse HttpClient::patch(const String &url)
    {
        return this->doSend(HttpMethod::HTTP_PATCH, url);
    }

    HttpResponse HttpClient::head(const String &url)
    {
        return this->doSend(HttpMethod::HTTP_HEAD, url);
    }

    HttpResponse HttpClient::options(const String &url)
    {
        return this->doSend(HttpMethod::HTTP_OPTIONS, url);
    }

    HttpResponse HttpClient::doSend(const HttpMethod &method, const String &url)
    {
        return this->_request.doSend(method, url);
    }

    HttpResponse HttpClient::doSend(const HttpRequest &request)
    {
        this->_request = request;
        return this->doSend(request.method, request.rawUrl);
    }
}