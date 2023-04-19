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

struct NodeStmtLet {
    const Token* tok_let;
    const Token* tok_ident;
    const Token* tok_eq;
    NodeExpr* expr;
    const Token* tok_semi;
    std::optional<NodeStmt*> next_stmt;
};

struct NodeStmtEq {
    const Token* tok_ident;
    const Token* tok_eq;
    const NodeExpr* expr;
    const Token* tok_semi;
    std::optional<NodeStmt*> next_stmt;
};

struct NodeStmtPrint {
    const Token* tok_print;
    const Token* tok_left_paren;
    NodeExpr* expr;
    const Token* tok_right_paren;
    const Token* tok_semi;
    std::optional<NodeStmt*> next_stmt;
};

struct NodeScope {
    const Token* tok_left_curly;
    std::optional<NodeStmt*> stmt;
    const Token* tok_right_curly;
};

struct NodeElse {
    const Token* tok_else;
    NodeScope* scope;
};

struct NodeStmtIf {
    const Token* tok_if;
    const Token* tok_left_paren;
    NodeExpr* expr;
    const Token* tok_right_paren;
    NodeScope* scope;
    std::optional<NodeElse*> else_;
    std::optional<NodeStmt*> next_stmt;
};

struct NodeStmt {
    std::variant<NodeStmtPrint*, NodeStmtLet*, NodeStmtEq*, NodeStmtIf*> var;
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