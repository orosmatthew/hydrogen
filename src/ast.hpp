#pragma once

#include "alloc.hpp"
#include "token.hpp"
#include <variant>

namespace ast {

struct NodeExpr;

struct NodeTermBaseParen {
    const Token* tok_left_paren;
    NodeExpr* expr;
    const Token* tok_right_paren;
};

struct NodeTermBaseNum {
    const Token* tok_num;
};

struct NodeTermBaseIdent {
    const Token* tok_ident;
};


struct NodeTermBase {
    std::variant<NodeTermBaseParen*, NodeTermBaseNum*, NodeTermBaseIdent*> var;
};

struct NodeTermNeg {
    const Token* tok_sign;
    NodeTermBase* term_base;
};

struct NodeTerm {
    std::variant<NodeTermNeg*, NodeTermBase*> var;
};

struct NodeExprBin {
    NodeExpr* lhs;
    const Token* tok_op;
    NodeExpr* rhs;
};

struct NodeExpr {
    std::variant<NodeTerm*, NodeExprBin*> var;
};

struct NodeStmt;

struct NodeStmtPred {
    NodeStmt* stmt;
};

struct NodeStmtLet {
    const Token* tok_let;
    const Token* tok_ident;
    const Token* tok_eq;
    NodeExpr* expr;
    const Token* tok_semi;
    std::optional<NodeStmtPred*> stmt_pred;
};

struct NodeStmtEq {
    const Token* tok_ident;
    const Token* tok_eq;
    const NodeExpr* expr;
    const Token* tok_semi;
    std::optional<NodeStmtPred*> stmt_pred;
};

struct NodeStmtPrint {
    const Token* tok_print;
    const Token* tok_left_paren;
    NodeExpr* expr;
    const Token* tok_right_paren;
    const Token* tok_semi;
    std::optional<NodeStmtPred*> stmt_pred;
};

struct NodeStmt {
    std::variant<NodeStmtPrint*, NodeStmtLet*, NodeStmtEq*> var;
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