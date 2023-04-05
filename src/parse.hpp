#pragma once

#include "ast.hpp"
#include "parse.hpp"
#include "token.hpp"

class Parser {
public:
    explicit Parser(std::vector<Token>&& tokens)
        : m_tokens(std::move(tokens))
        , index(0)
    {
    }

    std::optional<ast::NodeFactor*> parse_factor()
    {
        auto* factor = m_alloc.alloc<ast::NodeFactor>();
        if (!peak().has_value()) {
            return {};
        }
        if (peak().value()->type == TokenType::left_paren) {
            auto* node_paren = m_alloc.alloc<ast::NodeFactorParen>();
            node_paren->tok_left_paren = consume();
            if (auto ret = parse_expr()) {
                node_paren->expr = ret.value();
            }
            else {
                error("Expected expression");
            }
            if (peak().has_value() && peak().value()->type == TokenType::right_paren) {
                node_paren->tok_right_paren = consume();
            }
            else {
                error("Expected `)`");
            }
            factor->var = node_paren;
            return factor;
        }
        else if (peak().value()->type == TokenType::i64) {
            auto* node_pos = m_alloc.alloc<ast::NodeFactorPos>();
            node_pos->tok_num = consume();
            factor->var = node_pos;
            return factor;
        }
        else if (peak().value()->type == TokenType::sub) {
            auto* node_neg = m_alloc.alloc<ast::NodeFactorNeg>();
            node_neg->tok_sub = consume();
            node_neg->tok_num = consume();
            factor->var = node_neg;
            return factor;
        }
        return {};
    }

    std::optional<ast::NodeExprPred*> parse_expr_pred()
    {
        auto* expr_pred = m_alloc.alloc<ast::NodeExprPred>();
        if (!peak().has_value()) {
            return {};
        }
        if (peak().value()->type == TokenType::add) {
            auto* node_add = m_alloc.alloc<ast::NodeExprPredAdd>();
            node_add->tok_add = consume();
            if (auto ret = parse_term()) {
                node_add->term = ret.value();
            }
            else {
                error("Expected term");
            }
            node_add->expr_pred = parse_expr_pred();
            expr_pred->var = node_add;
            return expr_pred;
        }
        else if (peak().value()->type == TokenType::sub) {
            auto* node_sub = m_alloc.alloc<ast::NodeExprPredSub>();
            node_sub->tok_sub = consume();
            node_sub->term = parse_term().value();
            node_sub->expr_pred = parse_expr_pred();
            expr_pred->var = node_sub;
            return expr_pred;
        }
        return {};
    }

    std::optional<ast::NodeTermPred*> parse_term_pred()
    {
        auto* term_pred = m_alloc.alloc<ast::NodeTermPred>();
        if (!peak().has_value()) {
            return {};
        }
        if (peak().value()->type == TokenType::multi) {
            auto* node_multi = m_alloc.alloc<ast::NodeTermPredMulti>();
            node_multi->tok_multi = consume();
            if (auto ret = parse_factor()) {
                node_multi->factor = ret.value();
            }
            else {
                error("Expected factor");
            }
            node_multi->term_pred = parse_term_pred();
            term_pred->var = node_multi;
            return term_pred;
        }
        else if (peak().value()->type == TokenType::div) {
            auto* node_div = m_alloc.alloc<ast::NodeTermPredDiv>();
            node_div->tok_div = consume();
            node_div->factor = parse_factor().value();
            node_div->term_pred = parse_term_pred();
            term_pred->var = node_div;
            return term_pred;
        }
        return {};
    }

    std::optional<ast::NodeTerm*> parse_term()
    {
        auto* term = m_alloc.alloc<ast::NodeTerm>();
        if (auto factor = parse_factor()) {
            term->factor = factor.value();
        }
        else {
            return {};
        }
        term->term_pred = parse_term_pred();
        return term;
    }

    std::optional<ast::NodeExpr*> parse_expr()
    {
        auto* expr = m_alloc.alloc<ast::NodeExpr>();
        if (auto term = parse_term()) {
            expr->term = term.value();
        }
        else {
            return {};
        }
        expr->expr_pred = parse_expr_pred();
        return expr;
    }

    std::optional<ast::NodeStmt*> parse_stmt()
    {
        auto* stmt = m_alloc.alloc<ast::NodeStmt>();
        if (auto expr = parse_expr()) {
            stmt->expr = expr.value();
        }
        else {
            return {};
        }
        if (peak().has_value() && peak().value()->type == TokenType::semi) {
            stmt->tok_semi = consume();
        }
        else {
            error("Expected `;`");
        }
        stmt->stmt_pred = parse_stmt_pred();
        return stmt;
    }

    std::optional<ast::NodeStmtPred*> parse_stmt_pred()
    {
        auto stmt_pred = m_alloc.alloc<ast::NodeStmtPred>();
        auto ret = parse_stmt();
        if (!ret.has_value()) {
            return {};
        }
        stmt_pred->stmt = ret.value();
        return stmt_pred;
    }

private:
    std::optional<const Token*> peak()
    {
        if (index < m_tokens.size()) {
            return &m_tokens.at(index);
        }
        else {
            return {};
        }
    }

    const Token* consume()
    {
        index++;
        return &m_tokens.at(index - 1);
    }

    static void error(const std::string& msg)
    {
        std::cerr << "[Error] " << msg << std::endl;
        exit(EXIT_FAILURE);
    }

    Alloc m_alloc {};

    const std::vector<Token> m_tokens;
    size_t index;
};