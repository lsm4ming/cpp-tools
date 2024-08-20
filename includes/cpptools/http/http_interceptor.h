#pragma once

#include "server_request.h"
#include "response_writer.h"

namespace cpptools::http
{
    class InterceptContext
    {
    public:
        explicit InterceptContext(Request *request, HttpResponseWriter *responseWriter, const Function<void()> &next,
                                  const Function<void()> &abort)
        {
            this->request = request;
            this->responseWriter = responseWriter;
            this->next = next;
            this->abort = abort;
        }

        /**
         * 请求
         */
        Request *request;

        /**
         * 响应
         */
        HttpResponseWriter *responseWriter;

        /**
         * 继续执行
         */
        Function<void()> next;

        /**
         * 中断执行
         */
        Function<void()> abort;
    };

    class HttpInterceptor
    {
    public:
        /**
         * 拦截处理
         * @param context
         */
        virtual void interceptHandler(InterceptContext &context) const = 0;

        /**
         * 拦截路径匹配
         * @return 拦截路径
         */
        [[nodiscard]] virtual Vector<String> interceptPath() const = 0;

        /**
         * 拦截顺序
         * @return 权重
         */
        [[nodiscard]] virtual int order() const = 0;

        /**
         * 排序函数
         * @param other
         * @return
         */
        bool operator<(const HttpInterceptor &other) const
        {
            return this->order() < other.order();
        }
    };
}