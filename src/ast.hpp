#pragma once

#include "alloc.hpp"
#include "token.hpp"
#include <variant>

namespace ast {

struct NodeExpr;

struct NodeFactorParen {
    const Token* tok_left_paren;
    NodeExpr* expr;
    const Token* tok_right_paren;
};

struct NodeFactorPos {
    const Token* tok_num;
};

struct NodeFactorNeg {
    const Token* tok_sub;
    const Token* tok_num;
};

struct NodeFactor {
    std::variant<NodeFactorParen*, NodeFactorPos*, NodeFactorNeg*> var;
};

struct NodeTermPred;

struct NodeTermPredMulti {
    const Token* tok_multi;
    NodeFactor* factor;
    std::optional<NodeTermPred*> term_pred;
};

struct NodeTermPredDiv {
    const Token* tok_div;
    NodeFactor* factor;
    std::optional<NodeTermPred*> term_pred;
};

struct NodeTermPred {
    std::variant<NodeTermPredMulti*, NodeTermPredDiv*> var;
};

struct NodeTerm {
    NodeFactor* factor;
    std::optional<NodeTermPred*> term_pred;
};

struct NodeExprPred;

struct NodeExprPredAdd {
    const Token* tok_add;
    NodeTerm* term;
    std::optional<NodeExprPred*> expr_pred;
};

struct NodeExprPredSub {
    const Token* tok_sub;
    NodeTerm* term;
    std::optional<NodeExprPred*> expr_pred;
};

struct NodeExprPred {
    std::variant<NodeExprPredAdd*, NodeExprPredSub*> var;
};

struct NodeExpr {
    NodeTerm* term;
    std::optional<NodeExprPred*> expr_pred;
};

struct NodeStmt;

struct NodeStmtPred {
    NodeStmt* stmt;
};

struct NodeStmt {
    NodeExpr* expr;
    const Token* tok_semi;
    std::optional<NodeStmtPred*> stmt_pred;
};

// TODO
// void print_ast(const NodeBase& node, int level = 0)
//{
//    std::string bars;
//    for (int i = 0; i < level; i++) {
//        bars.append("| ");
//    }
//    std::cout << bars << to_string(node.type) << "\n";
//    bars.append("| ");
//    for (const NodeBase& child : node.children) {
//        if (child.type == ASTNodeType::terminal) {
//            std::cout << bars << child.token->value << "\n";
//        }
//        else {
//            print_ast(child, level + 1);
//        }
//    }
//}
}