#pragma once

#include "token.hpp"

namespace ast {

enum class ASTNodeType { none, expr, expr_pred, factor, term, term_pred, terminal };

std::string to_string(ASTNodeType type)
{
    switch (type) {
    case ASTNodeType::none:
        return "none";
    case ASTNodeType::expr:
        return "expr";
    case ASTNodeType::expr_pred:
        return "expr_pred";
    case ASTNodeType::factor:
        return "factor";
    case ASTNodeType::term:
        return "term";
    case ASTNodeType::term_pred:
        return "term_pred";
    case ASTNodeType::terminal:
        return "terminal";
    default:
        return "invalid";
    }
}

struct NodeBase {
    virtual ~NodeBase() = default;
    ASTNodeType type = ASTNodeType::none;
    const Token* token = nullptr;
    std::vector<NodeBase> children {};
};

struct NodeTerminal : public NodeBase {
    explicit NodeTerminal(const Token* tok)
    {
        token = tok;
        type = ASTNodeType::terminal;
    }
    [[nodiscard]] const Token* tok() const
    {
        return token;
    }
};

struct NodeExpr;

struct NodeFactor : public NodeBase {
    NodeFactor()
    {
        type = ASTNodeType::factor;
    }
    [[nodiscard]] std::optional<const std::reference_wrapper<NodeExpr>> expr() const
    {
        if (children.at(0).token->type == TokenType::left_paren)
            return (NodeExpr&)(children.at(1));
        else
            return {};
    }
    [[nodiscard]] std::optional<const Token*> u64() const
    {
        if (children.at(0).token->type == TokenType::u64)
            return children.at(0).token;
        else
            return {};
    }
};

struct NodeTermPred : public NodeBase {
    NodeTermPred()
    {
        type = ASTNodeType::term_pred;
    }
    [[nodiscard]] bool empty() const
    {
        return children.empty();
    }
    [[nodiscard]] std::optional<const Token*> op() const
    {
        if (children.empty())
            return {};
        else
            return children.at(0).token;
    }
    [[nodiscard]] std::optional<const std::reference_wrapper<NodeFactor>> factor() const
    {
        if (children.empty())
            return {};
        else
            return (NodeFactor&)(children.at(1));
    }
    [[nodiscard]] std::optional<const std::reference_wrapper<NodeTermPred>> term_pred() const
    {
        if (children.empty())
            return {};
        else
            return (NodeTermPred&)(children.at(2));
    }
};

struct NodeTerm : public NodeBase {
    NodeTerm()
    {
        type = ASTNodeType::term;
    }
    [[nodiscard]] const NodeFactor& factor() const
    {
        return (NodeFactor&)(children.at(0));
    }
    [[nodiscard]] const NodeTermPred& term_pred() const
    {
        return (NodeTermPred&)(children.at(1));
    }
};

struct NodeExprPred : public NodeBase {
    NodeExprPred()
    {
        type = ASTNodeType::expr_pred;
    }
    [[nodiscard]] bool empty() const
    {
        return children.empty();
    }
    [[nodiscard]] std::optional<const Token*> op() const
    {
        if (children.empty())
            return {};
        else
            return children.at(0).token;
    }
    [[nodiscard]] std::optional<const std::reference_wrapper<NodeTerm>> term() const
    {
        if (children.empty())
            return {};
        else
            return (NodeTerm&)(children.at(1));
    }
    [[nodiscard]] std::optional<const std::reference_wrapper<NodeExprPred>> expr_pred() const
    {
        if (children.empty())
            return {};
        else
            return (NodeExprPred&)(children.at(2));
    }
};

struct NodeExpr : public NodeBase {
public:
    NodeExpr()
    {
        type = ASTNodeType::expr;
    }
    [[nodiscard]] const NodeTerm& term() const
    {
        return (NodeTerm&)(children.at(0));
    }
    [[nodiscard]] const NodeExprPred& expr_pred() const
    {
        return (NodeExprPred&)(children.at(1));
    }
};

void print_ast(const NodeBase& node, int level = 0)
{
    std::string bars;
    for (int i = 0; i < level; i++) {
        bars.append("| ");
    }
    std::cout << bars << to_string(node.type) << "\n";
    bars.append("| ");
    for (const NodeBase& child : node.children) {
        if (child.type == ASTNodeType::terminal) {
            std::cout << bars << "-- " << child.token->value << "\n";
        }
        else {
            print_ast(child, level + 1);
        }
    }
}
}