#pragma once

#include "alloc.hpp"
#include "token.hpp"
#include <optional>
#include <variant>

namespace ast {

struct NodePostInc {
    const Token* tok_inc;
};

struct NodePostDec {
    const Token* tok_dec;
};

struct NodePost {
    std::variant<NodePostInc*, NodePostDec*> var;
};

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
    std::optional<NodePost*> post;
};

struct NodeTermBaseInc {
    const Token* tok_inc;
    const Token* tok_ident;
};

struct NodeTermBaseDec {
    const Token* tok_dec;
    const Token* tok_ident;
};

struct NodeTermBaseStr {
    const Token* tok_str_lit;
};

struct NodeTermBaseTrue {
    const Token* tok_true;
};

struct NodeTermBaseFalse {
    const Token* tok_false;
};

struct NodeTermBase {
    std::variant<
        NodeTermBaseParen*,
        NodeTermBaseNum*,
        NodeTermBaseIdent*,
        NodeTermBaseStr*,
        NodeTermBaseTrue*,
        NodeTermBaseFalse*,
        NodeTermBaseInc*,
        NodeTermBaseDec*>
        var;
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

struct NodeStmtVar {
    const Token* tok_var;
    const Token* tok_ident;
    const Token* tok_eq;
    NodeExpr* expr;
};

struct NodeStmtEq {
    const Token* tok_ident;
    const Token* tok_eq;
    NodeExpr* expr;
};

struct NodeStmtPrint {
    const Token* tok_print;
    const Token* tok_left_paren;
    NodeExpr* expr;
    const Token* tok_right_paren;
};

struct NodeBlock;

struct NodeScope {
    const Token* tok_left_curly;
    std::optional<NodeBlock*> block;
    const Token* tok_right_curly;
};

struct NodeElse {
    const Token* tok_else;
    NodeScope* scope;
};

struct NodeControlIf {
    const Token* tok_if;
    const Token* tok_left_paren;
    NodeExpr* expr;
    const Token* tok_right_paren;
    NodeScope* scope;
    std::optional<NodeElse*> else_;
};

struct NodeControlScope {
    NodeScope* scope;
};

struct NodeStmtWrite {
    const Token* tok_write;
    const Token* tok_left_paren;
    NodeExpr* expr1;
    const Token* tok_comma;
    NodeExpr* expr2;
    const Token* tok_right_paren;
};

struct NodeControlWhile {
    const Token* tok_while;
    const Token* tok_left_paren;
    NodeExpr* expr;
    const Token* tok_right_paren;
    NodeScope* scope;
};

struct NodeStmtBreak {
    const Token* tok_break;
};

struct NodeStmtExpr {
    NodeExpr* expr;
};

struct NodeControlFor {
    const Token* tok_for;
    const Token* tok_left_paren;
    NodeStmt* init_stmt;
    const Token* tok_semi_1;
    NodeExpr* expr;
    const Token* tok_semi_2;
    NodeStmt* loop_stmt;
    const Token* tok_right_paren;
    NodeScope* scope;
};

struct NodeStmt {
    std::variant<NodeStmtPrint*, NodeStmtVar*, NodeStmtEq*, NodeStmtWrite*, NodeStmtBreak*, NodeStmtExpr*> var;
};

struct NodeControl {
    std::variant<NodeControlIf*, NodeControlScope*, NodeControlWhile*, NodeControlFor*> var;
};

struct NodeBlockStmt {
    NodeStmt* stmt;
    const Token* tok_semi;
    std::optional<NodeBlock*> next;
};

struct NodeBlockControl {
    NodeControl* control;
    std::optional<NodeBlock*> next;
};

struct NodeBlock {
    std::variant<NodeBlockStmt*, NodeBlockControl*> var;
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