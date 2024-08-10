#pragma once

#include <utility>

#include "cpptools/common/common.h"

using namespace cpptools::common;

namespace cpptools::http
{
    class Node
    {
    private:
        std::string _pattern{};
        std::string _part{};
        std::vector<Node *> _children;
        bool _isWild{};

    public:
        explicit Node(String part, bool isWild = false) : _part(std::move(part)), _isWild(isWild)
        {}

        [[nodiscard]] String getPattern() const;

        void setPattern(const String &pattern);

        [[nodiscard]] String getPart() const;

        [[nodiscard]] bool isWild() const;

        [[nodiscard]] Node *matchChild(const std::string &part) const;

        [[nodiscard]] Vector<Node *> matchChildren(const String &part) const;

        void insert(const String &pattern, const Vector<String> &parts, int height);

        Node *search(const Vector<String> &parts, int height);
    };
}