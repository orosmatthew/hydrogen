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

    ast::NodeFactor* parse_factor()
    {
        auto* factor = m_alloc.alloc<ast::NodeFactor>();
        if (peak().has_value()) {
            if (peak().value()->type == TokenType::left_paren) {
                auto* node_paren = m_alloc.alloc<ast::NodeFactorParen>();
                node_paren->tok_left_paren = consume();
                node_paren->expr = parse_expr();
                if (peak().has_value() && peak().value()->type == TokenType::right_paren) {
                    node_paren->tok_right_paren = consume();
                }
                else {
                    std::cerr << "[Error] Expected `)`" << std::endl;
                    exit(EXIT_FAILURE);
                }
                factor->var = node_paren;
            }
            else if (peak().value()->type == TokenType::i64) {
                auto* node_pos = m_alloc.alloc<ast::NodeFactorPos>();
                node_pos->tok_num = consume();
                factor->var = node_pos;
            }
            else if (peak().value()->type == TokenType::sub) {
                auto* node_neg = m_alloc.alloc<ast::NodeFactorNeg>();
                node_neg->tok_sub = consume();
                node_neg->tok_num = consume();
                factor->var = node_neg;
            }
        }
        return factor;
    }

    ast::NodeExprPred* parse_expr_pred()
    {
        auto* expr_pred = m_alloc.alloc<ast::NodeExprPred>();
        if (peak().has_value()) {
            if (peak().value()->type == TokenType::add) {
                auto* node_add = m_alloc.alloc<ast::NodeExprPredAdd>();
                node_add->tok_add = consume();
                node_add->term = parse_term();
                node_add->expr_pred = parse_expr_pred();
                expr_pred->var = node_add;
                return expr_pred;
            }
            else if (peak().value()->type == TokenType::sub) {
                auto* node_sub = m_alloc.alloc<ast::NodeExprPredSub>();
                node_sub->tok_sub = consume();
                node_sub->term = parse_term();
                node_sub->expr_pred = parse_expr_pred();
                expr_pred->var = node_sub;
                return expr_pred;
            }
        }
        return expr_pred;
    }

    ast::NodeTermPred* parse_term_pred()
    {
        auto* term_pred = m_alloc.alloc<ast::NodeTermPred>();
        if (peak().has_value()) {
            if (peak().value()->type == TokenType::multi) {
                auto* node_multi = m_alloc.alloc<ast::NodeTermPredMulti>();
                node_multi->tok_multi = consume();
                node_multi->factor = parse_factor();
                node_multi->term_pred = parse_term_pred();
                term_pred->var = node_multi;
                return term_pred;
            }
            else if (peak().value()->type == TokenType::div) {
                auto* node_div = m_alloc.alloc<ast::NodeTermPredDiv>();
                node_div->tok_div = consume();
                node_div->factor = parse_factor();
                node_div->term_pred = parse_term_pred();
                term_pred->var = node_div;
                return term_pred;
            }
        }
        return term_pred;
    }

    ast::NodeTerm* parse_term()
    {
        auto* term = m_alloc.alloc<ast::NodeTerm>();
        term->factor = parse_factor();
        term->term_pred = parse_term_pred();
        return term;
    }

    ast::NodeExpr* parse_expr()
    {
        auto* expr = m_alloc.alloc<ast::NodeExpr>();
        expr->term = parse_term();
        expr->expr_pred = parse_expr_pred();
        return expr;
    }

    ast::NodeStmt* parse_stmt()
    {
        auto* stmt = m_alloc.alloc<ast::NodeStmt>();
        stmt->expr = parse_expr();
        if (peak().has_value() && peak().value()->type == TokenType::semi) {
            stmt->tok_semi = consume();
        }
        else {
            std::cerr << "[Error] Expected `;`" << std::endl;
            exit(EXIT_FAILURE);
        }
        stmt->stmt_pred = parse_stmt_pred();
        return stmt;
    }

    ast::NodeStmtPred* parse_stmt_pred()
    {
        auto stmt_pred = m_alloc.alloc<ast::NodeStmtPred>();
        m_was_consumed = false;
        ast::NodeExpr* node = parse_expr();
        if (!m_was_consumed) {
            return stmt_pred;
        }
        auto node_exist = m_alloc.alloc<ast::NodeStmtPredExist>();
        node_exist->expr = node;
        if (peak().has_value() && peak().value()->type == TokenType::semi) {
            node_exist->tok_semi = consume();
        }
        else {
            std::cerr << "[Error] Expected `;`" << std::endl;
            exit(EXIT_FAILURE);
        }
        node_exist->stmt_pred = parse_stmt_pred();
        stmt_pred->var = node_exist;
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
        m_was_consumed = true;
        index++;
        return &m_tokens.at(index - 1);
    }

    Alloc m_alloc {};
    bool m_was_consumed = false;

    const std::vector<Token> m_tokens;
    size_t index;
};