#pragma once

#include "parse.hpp"

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
            else {
                std::cerr << "[Error] Invalid token: " << peak().value()->value << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        else {
            std::cerr << "[Error] Unexpected end of tokens" << std::endl;
            exit(EXIT_FAILURE);
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

    const std::vector<Token> m_tokens;
    size_t index;
};