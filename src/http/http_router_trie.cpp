#include "cpptools/http/http_router_trie.h"

namespace cpptools::http
{

    void Node::setPattern(const std::string &pattern)
    {
        this->_pattern = pattern;
    }

    std::string Node::getPattern() const
    {
        return this->_pattern;
    }

    std::string Node::getPart() const
    {
        return this->_part;
    }

    bool Node::isWild() const
    {
        return this->_isWild;
    }

    Node *Node::matchChild(const std::string &part) const
    {
        for (Node *child: _children)
        {
            if (child->getPart() == part || child->isWild())
            {
                return child;
            }
        }
        return nullptr;
    }

    Vector<Node *> Node::matchChildren(const String &part) const
    {
        std::vector<Node *> nodes;
        for (Node *child: _children)
        {
            if (child->getPart() == part || child->isWild())
            {
                nodes.push_back(child);
            }
        }
        return nodes;
    }

    void Node::insert(const String &pattern, const Vector<String> &parts, int height)
    {
        if (parts.size() == height)
        {
            setPattern(pattern);
            return;
        }
        std::string part = parts[height];
        Node *child = matchChild(part);
        if (child == nullptr)
        {
            child = new Node(part, part[0] == ':' || part[0] == '*');
            _children.push_back(child);
        }
        child->insert(pattern, parts, height + 1);
    }

    Node *Node::search(const Vector<String> &parts, int height)
    {
        if (parts.size() == height || getPart()[0] == '*')
        {
            if (getPattern().empty())
            {
                return nullptr;
            }
            return this;
        }
        const String& part = parts[height];
        Vector<Node *> matchedChildren = matchChildren(part);
        for (Node *child: matchedChildren)
        {
            Node *result = child->search(parts, height + 1);
            if (result != nullptr)
            {
                return result;
            }
        }
        return nullptr;
    }
}
