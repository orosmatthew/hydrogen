#pragma once

#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

enum class TokenType {
    none,
    int_lit,
    add,
    sub,
    multi,
    div,
    left_paren,
    right_paren,
    semi,
    var,
    ident,
    eq,
    print,
    lt,
    gt,
    lte,
    gte,
    if_,
    else_,
    left_curly,
    right_curly,
    deq,
    neq,
    str_lit,
    comma,
    write,
    true_,
    false_,
    while_,
    break_,
    inc,
    dec,
    for_,
};

enum class BinAssoc { none, left, right };

inline BinAssoc bin_assoc(TokenType type)
{
    switch (type) {
    case TokenType::multi:
    case TokenType::div:
    case TokenType::add:
    case TokenType::sub:
    case TokenType::lt:
    case TokenType::gt:
    case TokenType::lte:
    case TokenType::gte:
        return BinAssoc::left;
    default:
        return BinAssoc::none;
    }
}

inline int bin_prec(TokenType type)
{
    switch (type) {
    case TokenType::multi:
    case TokenType::div:
        return 3;
    case TokenType::add:
    case TokenType::sub:
        return 2;
    case TokenType::lt:
    case TokenType::gt:
    case TokenType::lte:
    case TokenType::gte:
        return 1;
    case TokenType::deq:
        return 0;
    default:
        return -1;
    }
}

inline bool is_bin_op(TokenType type)
{
    switch (type) {
    case TokenType::add:
    case TokenType::sub:
    case TokenType::multi:
    case TokenType::div:
    case TokenType::lt:
    case TokenType::gt:
    case TokenType::lte:
    case TokenType::gte:
    case TokenType::deq:
    case TokenType::neq:
        return true;
    default:
        return false;
    }
}

inline std::string to_string(TokenType type)
{
    switch (type) {
    case TokenType::none:
        return "none";
    case TokenType::int_lit:
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
    case TokenType::var:
        return "var";
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

inline std::vector<Token> tokenize_file(const std::filesystem::path& path)
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
            tokens.push_back({ TokenType::int_lit, num_buf });
        }
        else if (isalpha(source[i]) || source[i] == '_') {
            std::string buf;
            do {
                buf.push_back(source[i]);
                i++;
            } while (isalnum(source[i]) || source[i] == '_');
            i--;
            if (buf == "var") {
                tokens.push_back({ TokenType::var, buf });
            }
            else if (buf == "print") {
                tokens.push_back({ TokenType::print, buf });
            }
            else if (buf == "if") {
                tokens.push_back({ TokenType::if_, buf });
            }
            else if (buf == "else") {
                tokens.push_back({ TokenType::else_, buf });
            }
            else if (buf == "write") {
                tokens.push_back({ TokenType::write, buf });
            }
            else if (buf == "true") {
                tokens.push_back({ TokenType::true_, buf });
            }
            else if (buf == "false") {
                tokens.push_back({ TokenType::false_, buf });
            }
            else if (buf == "while") {
                tokens.push_back({ TokenType::while_, buf });
            }
            else if (buf == "break") {
                tokens.push_back({ TokenType::break_, buf });
            }
            else if (buf == "for") {
                tokens.push_back({ TokenType::for_, buf });
            }
            else {
                tokens.push_back({ TokenType::ident, buf });
            }
        }
        else if (source[i] == '+') {
            if (source[i + 1] == '+') {
                tokens.push_back({ TokenType::inc, "++" });
                i++;
                continue;
            }
            else {
                tokens.push_back({ TokenType::add, "+" });
            }
        }
        else if (source[i] == '-') {
            if (source[i + 1] == '-') {
                tokens.push_back({ TokenType::dec, "--" });
                i++;
                continue;
            }
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
            if (source[i + 1] == '=') {
                tokens.push_back({ TokenType::deq, "==" });
                i++;
            }
            else {
                tokens.push_back({ TokenType::eq, "=" });
            }
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
        else if (source[i] == '{') {
            tokens.push_back({ TokenType::left_curly, "{" });
        }
        else if (source[i] == '}') {
            tokens.push_back({ TokenType::right_curly, "}" });
        }
        else if (source[i] == '!' && source[i + 1] == '=') {
            tokens.push_back({ TokenType::neq, "!=" });
            i++;
            continue;
        }
        else if (source[i] == '"') {
            i++;
            std::string buf;
            while (source[i] != '"') {
                if (source[i] == '\\') {
                    switch (source[i + 1]) {
                    case 'n':
                        buf.push_back('\n');
                        i += 2;
                        break;
                    default:
                        std::cerr << "[Error] Unknown escape" << std::endl;
                        exit(EXIT_FAILURE);
                    }
                    continue;
                }
                buf.push_back(source[i]);
                i++;
            }
            tokens.push_back({ TokenType::str_lit, buf });
        }
        else if (source[i] == ',') {
            tokens.push_back({ TokenType::comma, "," });
            i++;
        }
        else if (source[i] != ' ' && source[i] != '\n' && source[i] != '\r' && source[i] != '\t') {
            std::cerr << "[Error] Unexpected token: `" << source[i] << "`" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    return tokens;
}