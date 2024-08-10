#include <utility>

#include "cpptools/common/flag.h"

namespace cpptools::common
{
    static Usage usage_ = nullptr;
    static HashMap<String, Opt> opts_;

    void Parse(int argc, char *argv[])
    {
        if (nullptr == usage_)
        {
            printf("usage function not set\n");
            exit(-1);
        }
        int parseIndex = 1;  // 这里跳过命令名不解析，所以parseIndex从1开始
        while (parseIndex < argc)
        {
            if (ParseOpt(argc, argv, parseIndex))
            {
                exit(-2);
            }
        }
        if (not CheckRequired())
        {  // 校验必设选项，非必设的则设置默认值
            usage_();
            exit(-1);
        }
    }

    bool isValidName(const String &name)
    {
        if (name.empty()) return false;
        if (name[0] == '-')
        {
            printf("option %s begins with -\n", name.c_str());
            return false;
        }
        if (name.find('=') != String::npos)
        {
            printf("option %s contains =\n", name.c_str());
            return false;
        }
        return true;
    }

    int ParseOpt(int argc, char **argv, int &parseIndex)
    {
        char *opt = argv[parseIndex];
        int optLen = (int) strlen(opt);
        if (optLen <= 1)
        {  // 选项的长度必须>=2
            printf("option's len must greater than or equal to 2\n");
            return -1;
        }
        if (opt[0] != '-')
        {  // 选项必须以'-'开头
            printf("option must begins with '-', %s is invalid option\n", opt);
            return -1;
        }
        opt++;  // 过滤第一个'-'
        optLen--;
        if (*opt == '-')
        {
            opt++;  // 过滤第二个'-'
            optLen--;
        }
        // 过滤完有效的'-'之后还要再check一下后面的内容和长度
        if (optLen == 0 || *opt == '-' || *opt == '=')
        {
            printf("bad opt syntax:%s\n", argv[parseIndex]);
            return -1;
        }
        // 执行到这里说明是一个选项，接下来判断这个选项是否有参数
        bool hasArgument = false;
        String argument;
        for (int i = 1; i < optLen; i++)
        {
            if (opt[i] == '=')
            {
                hasArgument = true;
                argument = String(opt + i + 1);  // 取等号之后的内容赋值为argument
                opt[i] = 0;  // 这样opt指向的字符串就是'='之前的内容。
                break;
            }
        }
        String optName = String(opt);
        if (optName == "help" || optName == "h")
        {  // 有help选项，则直接调用usage_函数，并退出
            if (usage_) usage_();
            exit(0);
        }
        auto iter = opts_.find(optName);
        if (iter == opts_.end())
        {  // 选项不存在
            printf("option provided but not defined: -%s\n", optName.c_str());
            return -1;
        }
        if (iter->second.IsBoolOpt())
        {  // 不需要参数的bool类型选项
            iter->second.SetBoolValue(true);
            parseIndex++;  // parseIndex跳到下一个选项
        } else
        {  // 需要参数的选项，参数可能在下一个命令行参数中
            if (hasArgument)
            {
                parseIndex++;
            } else
            {
                if (parseIndex + 1 < argc)
                {  // 选项的值在下一个命令行参数
                    hasArgument = true;
                    argument = String(argv[parseIndex + 1]);
                    parseIndex += 2;  // parseIndex跳到下一个选项
                }
            }
            if (not hasArgument)
            {
                printf("option needs an argument: -%s\n", optName.c_str());
                return -1;
            }
            iter->second.SetValue(argument);
        }
        return 0;
    }

    bool CheckRequired()
    {
        auto iter = opts_.begin();
        while (iter != opts_.end())
        {
            if (!iter->second.CheckRequired()) return false;
            iter++;
        }
        return true;
    }

    void setOptCheck(const std::string &name)
    {
        if (opts_.find(name) != opts_.end())
        {
            printf("%s opt already set\n", name.c_str());
            exit(-1);
        }
        if (not isValidName(name))
        {
            printf("%s is invalid name\n", name.c_str());
            exit(-2);
        }
    }

    void BoolOpt(bool *value, const std::string &name)
    {
        setOptCheck(name);
        opts_[name] = Opt(value, name, false, false);
    }

    void Int64Opt(int64_t *value, const std::string &name, int64_t defaultValue)
    {
        setOptCheck(name);
        opts_[name] = Opt(value, name, defaultValue, false);
    }

    void StrOpt(std::string *value, const std::string &name, const std::string &defaultValue)
    {
        setOptCheck(name);
        opts_[name] = Opt(value, name, defaultValue, false);
    }

    void Int64OptRequired(int64_t *value, const std::string &name)
    {
        setOptCheck(name);
        opts_[name] = Opt(value, name, 0, true);
    }

    void StrOptRequired(std::string *value, const std::string &name)
    {
        setOptCheck(name);
        opts_[name] = Opt(value, name, "", true);
    }

    void SetUsage(Usage usage)
    { usage_ = std::move(usage); }
}