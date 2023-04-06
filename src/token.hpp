#pragma once

#include <cctype>
enum class TokenType {
    none,
    i64,
    add,
    sub,
    multi,
    div,
    left_paren,
    right_paren,
    semi,
    let,
    ident,
    eq,
    print,
    lt,
    gt,
    lte,
    gte
};

std::string to_string(TokenType type)
{
    switch (type) {
    case TokenType::none:
        return "none";
    case TokenType::i64:
        return "pos_i64";
    case TokenType::add:
        return "add";
    case TokenType::sub:
        return "sub";
    case TokenType::multi:
        return "multi";
    case TokenType::div:
        return "div";
    case TokenType::left_paren:
        return "left_paren";
    case TokenType::right_paren:
        return "right_paren";
    case TokenType::semi:
        return "semi";
    case TokenType::let:
        return "let";
    case TokenType::ident:
        return "ident";
    case TokenType::eq:
        return "eq";
    case TokenType::print:
        return "print";
    default:
        return "invalid";
    }
}

struct Token {
    TokenType type;
    std::string value;
};

std::vector<Token> tokenize_file(const std::filesystem::path& path)
{
    std::fstream file(path.c_str(), std::ios::in);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    std::vector<Token> tokens;
    for (size_t i = 0; i < source.size(); i++) {
        if (isdigit(source[i])) {
            std::string num_buf;
            do {
                num_buf.push_back(source[i]);
                i++;
            } while (isdigit(source[i]));
            i--;
            tokens.push_back({ TokenType::i64, num_buf });
        }
        else if (isalpha(source[i])) {
            std::string buf;
            do {
                buf.push_back(source[i]);
                i++;
            } while (isalnum(source[i]));
            i--;
            if (buf == "let") {
                tokens.push_back({ TokenType::let, buf });
            }
            else if (buf == "print") {
                tokens.push_back({ TokenType::print, buf });
            }
            else {
                tokens.push_back({ TokenType::ident, buf });
            }
        }
        else if (source[i] == '+') {
            tokens.push_back({ TokenType::add, "+" });
        }
        else if (source[i] == '-') {
            tokens.push_back({ TokenType::sub, "-" });
        }
        else if (source[i] == '*') {
            tokens.push_back({ TokenType::multi, "*" });
        }
        else if (source[i] == '/') {
            tokens.push_back({ TokenType::div, "/" });
        }
        else if (source[i] == '(') {
            tokens.push_back({ TokenType::left_paren, "(" });
        }
        else if (source[i] == ')') {
            tokens.push_back({ TokenType::right_paren, ")" });
        }
        else if (source[i] == ';') {
            tokens.push_back({ TokenType::semi, ";" });
        }
        else if (source[i] == '=') {
            tokens.push_back({ TokenType::eq, "=" });
        }
        else if (source[i] == '<') {
            if (source[i + 1] == '=') {
                tokens.push_back({ TokenType::lte, "<=" });
                i++;
            }
            else {
                tokens.push_back({ TokenType::lt, "<" });
            }
        }
        else if (source[i] == '>') {
            if (source[i + 1] == '=') {
                tokens.push_back({ TokenType::gte, ">=" });
                i++;
            }
            else {
                tokens.push_back({ TokenType::gt, ">" });
            }
        }
        else if (source[i] != ' ' && source[i] != '\n' && source[i] != '\r') {
            std::cerr << "[Error] Unexpected token: `" << source[i] << "`" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    return tokens;
}