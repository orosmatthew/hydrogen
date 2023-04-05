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
    NodeTermPred* term_pred;
};

struct NodeTermPredDiv {
    const Token* tok_div;
    NodeFactor* factor;
    NodeTermPred* term_pred;
};

struct NodeTermPred {
    std::optional<std::variant<NodeTermPredMulti*, NodeTermPredDiv*>> var;
};

struct NodeTerm {
    NodeFactor* factor;
    NodeTermPred* term_pred;
};

struct NodeExprPred;

struct NodeExprPredAdd {
    const Token* tok_add;
    NodeTerm* term;
    NodeExprPred* expr_pred;
};

struct NodeExprPredSub {
    const Token* tok_sub;
    NodeTerm* term;
    NodeExprPred* expr_pred;
};

struct NodeExprPred {
    std::optional<std::variant<NodeExprPredAdd*, NodeExprPredSub*>> var;
};

struct NodeExpr {
    NodeTerm* term;
    NodeExprPred* expr_pred;
};

struct NodeStmt;
struct NodeStmtPredExist;

struct NodeStmtPred {
    std::optional<NodeStmtPredExist*> var;
};

struct NodeStmtPredExist {
    NodeExpr* expr;
    const Token* tok_semi;
    NodeStmtPred* stmt_pred;
};

struct NodeStmt {
    NodeExpr* expr;
    const Token* tok_semi;
    NodeStmtPred* stmt_pred;
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