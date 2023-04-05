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

    ast::NodeFactor parse_factor()
    {
        ast::NodeFactor factor;
        if (peak().has_value()) {
            if (peak().value()->type == TokenType::left_paren) {
                factor.children.push_back(ast::NodeTerminal(consume()));
                factor.children.push_back(parse_expr());
                if (peak().has_value() && peak().value()->type == TokenType::right_paren) {
                    factor.children.push_back(ast::NodeTerminal(consume()));
                }
                else {
                    std::cerr << "[Error] Expected `)`" << std::endl;
                    exit(EXIT_FAILURE);
                }
            }
            else if (peak().value()->type == TokenType::i64) {
                factor.children.push_back(ast::NodeTerminal(consume()));
            }
            else if (peak().value()->type == TokenType::sub) {
                factor.children.push_back(ast::NodeTerminal(consume()));
                factor.children.push_back(ast::NodeTerminal(consume()));
            }
        }
        return factor;
    }

    ast::NodeExprPred parse_expr_pred()
    {
        ast::NodeExprPred expr_pred;
        if (peak().has_value()) {
            if (peak().value()->type == TokenType::add || peak().value()->type == TokenType::sub) {
                expr_pred.children.push_back(ast::NodeTerminal(consume()));
                expr_pred.children.push_back(parse_term());
                expr_pred.children.push_back(parse_expr_pred());
                return expr_pred;
            }
        }
        return expr_pred;
    }

    ast::NodeTermPred parse_term_pred()
    {
        ast::NodeTermPred term_pred;
        if (peak().has_value()) {
            if (peak().value()->type == TokenType::multi || peak().value()->type == TokenType::div) {
                term_pred.children.push_back(ast::NodeTerminal(consume()));
                term_pred.children.push_back(parse_factor());
                term_pred.children.push_back(parse_term_pred());
                return term_pred;
            }
        }
        return term_pred;
    }

    ast::NodeTerm parse_term()
    {
        ast::NodeTerm term;
        term.children.push_back(parse_factor());
        term.children.push_back(parse_term_pred());
        return term;
    }

    ast::NodeExpr parse_expr()
    {
        ast::NodeExpr expr;
        expr.children.push_back(parse_term());
        expr.children.push_back(parse_expr_pred());
        return expr;
    }

    ast::NodeStmt parse_stmt()
    {
        ast::NodeStmt stmt;
        stmt.children.push_back(parse_expr());
        if (peak().has_value() && peak().value()->type == TokenType::semi) {
            stmt.children.push_back(ast::NodeTerminal(consume()));
        }
        else {
            std::cerr << "[Error] Expected `;`" << std::endl;
            exit(EXIT_FAILURE);
        }
        stmt.children.push_back(parse_stmt_pred());

        return stmt;
    }

    ast::NodeStmtPred parse_stmt_pred()
    {
        ast::NodeStmtPred stmt_pred;
        m_was_consumed = false;
        auto node = parse_expr();
        if (!m_was_consumed) {
            return stmt_pred;
        }
        stmt_pred.children.push_back(node);
        if (peak().has_value() && peak().value()->type == TokenType::semi) {
            stmt_pred.children.push_back(ast::NodeTerminal(consume()));
        }
        else {
            std::cerr << "[Error] Expected `;`" << std::endl;
            exit(EXIT_FAILURE);
        }
        stmt_pred.children.push_back(parse_stmt_pred());
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

    bool m_was_consumed = false;

    const std::vector<Token> m_tokens;
    size_t index;
};