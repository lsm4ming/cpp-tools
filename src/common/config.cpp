#include "cpptools/common/config.h"

namespace cpptools::common
{
    bool IniConfig::load(const String &path)
    {
        if (path.empty()) return false;
        std::ifstream in;
        std::string line;
        in.open(path.c_str());
        if (not in.is_open()) return false;

        // 考虑reload，清空之前的配置
        this->_cfg.clear();

        while (getline(in, line))
        {
            std::string section, key, value;
            if (not parseLine(line, section, key, value))
            {
                continue;
            }
            setSectionKeyValue(section, key, value);
        }
        return true;
    }

    bool IniConfig::parseLine(String &line, String &section, String &key, String &value)
    {
        static std::string curSection;
        std::string nodes[2] = {"#", ";"};  //去掉注释的内容
        for (const auto &node: nodes)
        {
            std::string::size_type pos = line.find(node);
            if (pos != std::string::npos) line.erase(pos);
        }
        Strings::trim(line);
        if (line.empty()) return false;
        if (line[0] == '[' && line[line.size() - 1] == ']')
        {
            section = line.substr(1, line.size() - 2);
            Strings::trim(section);
            curSection = section;
            return false;
        }
        if (curSection.empty()) return false;
        bool isKey = true;
        for (char i: line)
        {
            if (i == '=')
            {
                isKey = false;
                continue;
            }
            if (isKey)
            {
                key += i;
            } else
            {
                value += i;
            }
        }
        section = curSection;
        Strings::trim(key);
        Strings::trim(value);
        return true;
    }

    void IniConfig::setSectionKeyValue(String &section, String &key, String &value)
    {
        if (this->_cfg.find(section) == this->_cfg.end())
        {
            SortMap<String, String> kvMap;
            this->_cfg[section] = kvMap;
        }
        if (!key.empty() && !value.empty()) this->_cfg[section][key] = value;
    }

    String IniConfig::getStrValue(const String &section, const String &key, const String &defaultValue) const
    {
        if (this->_cfg.find(section) == this->_cfg.end())
        {
            return defaultValue;
        }
        auto sectionMap = this->_cfg.find(section);
        if (sectionMap == this->_cfg.end())
        {
            return defaultValue;
        }
        auto valueMap = sectionMap->second.find(key);
        if (valueMap == sectionMap->second.end())
        {
            return defaultValue;
        }
        return valueMap->second;
    }

    int64 IniConfig::getIntValue(const String &section, const String &key, int64 defaultValue) const
    {
        auto value = this->getStrValue(section, key, std::to_string(defaultValue));
        return std::stoll(value);
    }

    size_t IniConfig::save(const String &path) const
    {
        std::ofstream out;
        out.open(path.c_str());
        if (not out.is_open()) return 0;
        for (const auto &section: this->_cfg)
        {
            out << '[' << section.first << ']' << std::endl;
            for (const auto &key: section.second)
            {
                out << key.first << '=' << key.second << std::endl;
            }
            out << std::endl;
        }
        // 获取写入的文件大小
        out.close();
        struct stat buffer{};
        if (stat(path.c_str(), &buffer) == 0)
        {
            return buffer.st_size;
        }
        return 0;
    }
}


