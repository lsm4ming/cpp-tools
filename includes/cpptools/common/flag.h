#pragma once

#include <cstring>
#include "common.h"

namespace cpptools::common
{
    using Usage = Function<void()>;

    // 设置非必须的bool选项
    void BoolOpt(bool *value, const String &name);

    // 设置非必须的int64_t选项
    void Int64Opt(int64_t *value, const String &name, int64_t defaultValue);

    // 设置非必须的string选项
    void StrOpt(String *value, const String &name, const String &defaultValue);

    // 设置必须的int64_t选项
    void Int64OptRequired(int64_t *value, const String &name);

    // 设置必须的string选项
    void StrOptRequired(String *value, const String &name);

    // 设置用法
    void SetUsage(Usage usage);

    // 解析参数
    void Parse(int argc, char *argv[]);

    enum OptType
    {
        INT64_T = 1,
        BOOL = 2,
        STRING = 3,
    };

    class Opt
    {
    public:
        Opt() = default;

        Opt(bool *value, String name, bool defaultValue, bool required)
        {
            init(BOOL, value, std::move(name), required);
            *(bool *) value_ = defaultValue;
        }

        Opt(int64_t *value, String name, int64_t defaultValue, bool required)
        {
            init(INT64_T, value, std::move(name), required);
            *(int64_t *) value_ = defaultValue;
        }

        Opt(String *value, String name, String defaultValue, bool required)
        {
            init(STRING, value, std::move(name), required);
            *(String *) value_ = std::move(defaultValue);
        }

        bool IsBoolOpt()
        { return type_ == BOOL; }

        void SetBoolValue(bool value)
        {
            value_is_set_ = true;
            *(bool *) value_ = value;
        }

        void SetValue(const String &value)
        {
            if (type_ == STRING)
            {
                *(String *) value_ = value;
            } else if (type_ == INT64_T)
            {
                *(int64_t *) value_ = std::stoll(value);
            }
            value_is_set_ = true;
        }

        bool CheckRequired()
        {
            if (!required_ || value_is_set_) return true;
            printf("required option %s not set argument\n", name_.c_str());
            return false;
        }

    private:
        void init(OptType type, void *value, String name, bool required)
        {
            type_ = type;
            name_ = std::move(name);
            value_ = (void *) value;
            required_ = required;
            if (required_) value_is_set_ = false;
        }

    private:
        OptType type_{};
        String name_{};
        void *value_{nullptr};
        bool value_is_set_{true};
        bool required_{false};
    };

    static bool isValidName(const String &name);

    static int ParseOpt(int argc, char *argv[], int &parseIndex);

    static bool CheckRequired();

    static void setOptCheck(const String &name);
}