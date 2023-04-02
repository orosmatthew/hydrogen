#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <vector>

enum class TokenType { none, u64, add, sub, left_paren, right_paren };

std::string to_string(TokenType type)
{
    switch (type) {
    case TokenType::none:
        return "none";
    case TokenType::u64:
        return "u64";
    case TokenType::add:
        return "add";
    case TokenType::sub:
        return "sub";
    case TokenType::left_paren:
        return "left_paren";
    case TokenType::right_paren:
        return "right_paren";
    default:
        return "invalid";
    }
}

struct Token {
    TokenType type;
    std::string value;
};

enum class ASTNodeType { none, expr, expr_pred, factor, term };

std::string to_string(ASTNodeType type)
{
    switch (type) {
    case ASTNodeType::none:
        return "none";
    case ASTNodeType::expr:
        return "expr";
    case ASTNodeType::expr_pred:
        return "expr_pred";
    case ASTNodeType::factor:
        return "factor";
    case ASTNodeType::term:
        return "term";
    default:
        return "invalid";
    }
}

struct ASTNode {
    ASTNodeType type;
    const Token* token;
    std::vector<ASTNode> children;
};

namespace gen {
void print_u64_def(std::fstream& file)
{
    file << ";; -- print_u64_def --\n";
    file << "print_u64:\n";
    file << "    sub rsp, 40\n";
    file << "    xor ecx, ecx\n";
    file << "    mov rsi, -3689348814741910323\n";
    file << ".LBB0_1:\n";
    file << "    mov rax, rdi\n";
    file << "    mul rsi\n";
    file << "    shr rdx, 3\n";
    file << "    lea eax, [rdx + rdx]\n";
    file << "    lea eax, [rax + 4*rax]\n";
    file << "    mov r8d, edi\n";
    file << "    sub r8d, eax\n";
    file << "    or  r8b, 48\n";
    file << "    mov byte [rsp + rcx + 31], r8b\n";
    file << "    dec rcx\n";
    file << "    cmp rdi, 9\n";
    file << "    mov rdi, rdx\n";
    file << "    ja  .LBB0_1\n";
    file << "    lea rsi, [rsp + rcx]\n";
    file << "    add rsi, 32\n";
    file << "    neg rcx\n";
    file << "    mov edi, 1\n";
    file << "    mov rdx, rcx\n";
    file << "    mov rax, 1\n";
    file << "    syscall\n";
    file << "    add rsp, 40\n";
    file << "    ret\n";
}

void print_u64(std::fstream& file)
{
    file << "    call print_u64\n";
}

void print_newline(std::fstream& file)
{
    file << "    ;; -- print_newline --\n";
    file << "    push 0xA\n";
    file << "    mov rax, 1\n";
    file << "    mov rdi, 1\n";
    file << "    mov rsi, rsp\n";
    file << "    mov rdx, 1\n";
    file << "    syscall\n";
    file << "    add rsp, 1\n";
}

void start(std::fstream& file)
{
    file << ";; -- start --\n";
    file << "section .text\n";
    file << "global _start\n";
    file << "_start:\n";
}

void exit(std::fstream& file)
{
    file << "    ;; -- exit --\n";
    file << "    mov rax, 0x3c\n";
    file << "    mov rdi, 0\n";
    file << "    syscall\n";
}

void add(std::fstream& file, const Token& a, const Token& b)
{
    if (a.type != TokenType::u64 || b.type != TokenType::u64) {
        std::cerr << "[Error] Invalid addition parameters" << std::endl;
    }
    file << "    ;; -- add --\n";
    file << "    mov rax, " << std::stoul(a.value) << "\n";
    file << "    add rax, " << std::stoul(b.value) << "\n";
}

void sub(std::fstream& file, const Token& a, const Token& b)
{
    if (a.type != TokenType::u64 || b.type != TokenType::u64) {
        std::cerr << "[Error] Invalid subtraction parameters" << std::endl;
    }
    file << "    ;; -- sub --\n";
    file << "    mov rax, " << std::stoul(a.value) << "\n";
    file << "    sub rax, " << std::stoul(b.value) << "\n";
}

void statement(std::fstream& file, const Token& a, const Token& op, const Token& b)
{
    switch (op.type) {
    case TokenType::add:
        add(file, a, b);
        break;
    case TokenType::sub:
        sub(file, a, b);
        break;
    default:
        std::cerr << "[Error] Invalid statement operation" << std::endl;
        ::exit(EXIT_FAILURE);
    }
}

void ast_expr(std::fstream& file, const ASTNode& expr);

void ast_factor(std::fstream& file, const ASTNode& factor)
{
    if (factor.children.at(0).token->type == TokenType::left_paren) {
        ast_expr(file, factor.children.at(1));
    }
    else {
        file << "    push " << std::stoul(factor.children.at(0).token->value) << "\n";
    }
}

void ast_expr_pred(std::fstream& file, const ASTNode& expr_pred)
{
    if (expr_pred.children.empty()) {
        return;
    }
    ast_factor(file, expr_pred.children.at(1));
    if (expr_pred.children.at(0).token->type == TokenType::add) {
        file << "    pop rcx\n";
        file << "    pop rdx\n";
        file << "    add rdx, rcx\n";
        file << "    push rdx\n";
    }
    else if (expr_pred.children.at(0).token->type == TokenType::sub) {
        file << "    pop rcx\n";
        file << "    pop rdx\n";
        file << "    sub rdx, rcx\n";
        file << "    push rdx\n";
    }
    ast_expr_pred(file, expr_pred.children.at(2));
}

void ast_expr(std::fstream& file, const ASTNode& expr)
{
    ast_factor(file, expr.children.at(0));
    ast_expr_pred(file, expr.children.at(1));
}
} // namespace gen

class Parser {
public:
    explicit Parser(std::vector<Token>&& tokens)
        : m_tokens(std::move(tokens))
        , index(0)
    {
    }

    ASTNode parse_factor()
    {
        ASTNode factor { ASTNodeType::factor };
        if (peak().has_value()) {
            if (peak().value()->type == TokenType::left_paren) {
                factor.children.emplace_back(ASTNodeType::term, consume());
                factor.children.push_back(parse_expr());
                if (peak().has_value() && peak().value()->type == TokenType::right_paren) {
                    factor.children.emplace_back(ASTNodeType::term, consume());
                }
                else {
                    std::cerr << "[Error] Unmatched parentheses in factor" << std::endl;
                    exit(EXIT_FAILURE);
                }
            }
            else if (peak().value()->type == TokenType::u64) {
                factor.children.emplace_back(ASTNodeType::term, consume());
            }
            else {
                std::cerr << "[Error] Invalid factor: " << peak().value()->value << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        else {
            std::cerr << "[Error] Unexpected end of tokens" << std::endl;
            exit(EXIT_FAILURE);
        }
        return factor;
    }

    ASTNode parse_expr_pred()
    {
        ASTNode expr_pred { ASTNodeType::expr_pred };
        if (peak().has_value()) {
            if (peak().value()->type == TokenType::add) {
                expr_pred.children.emplace_back(ASTNodeType::term, consume());
                expr_pred.children.push_back(parse_factor());
                expr_pred.children.push_back(parse_expr_pred());
            }
            else if (peak().value()->type == TokenType::sub) {
                expr_pred.children.emplace_back(ASTNodeType::term, consume());
                expr_pred.children.push_back(parse_factor());
                expr_pred.children.push_back(parse_expr_pred());
            }
            else {
                return { ASTNodeType::none };
            }
        }
        return expr_pred;
    }

    ASTNode parse_expr()
    {
        ASTNode expr { ASTNodeType::expr };
        expr.children.push_back(parse_factor());
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
            tokens.push_back({ TokenType::u64, num_buf });
        }
        else if (source[i] == '+') {
            tokens.push_back({ TokenType::add, "+" });
        }
        else if (source[i] == '-') {
            tokens.push_back({ TokenType::sub, "-" });
        }
        else if (source[i] == '(') {
            tokens.push_back({ TokenType::left_paren, "(" });
        }
        else if (source[i] == ')') {
            tokens.push_back({ TokenType::right_paren, ")" });
        }
        else if (source[i] != ' ') {
            std::cerr << "[Error] Unexpected token: " << source[i] << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    return tokens;
}

void print_ast(const ASTNode& node, int level = 0)
{
    std::string bars;
    for (int i = 0; i < level; i++) {
        bars.append("| ");
    }
    std::cout << bars << to_string(node.type) << "\n";
    bars.append("| ");
    for (const ASTNode& child : node.children) {
        if (child.type == ASTNodeType::term) {
            std::cout << bars << "-- " << child.token->value << "\n";
        }
        else {
            print_ast(child, level + 1);
        }
    }
}

int main(int argc, const char* argv[])
{
    if (argc != 2) {
        std::cerr << "[Error] Invalid number of arguments\n";
        std::cout << "[Usage] hydro <input.hy>\n";
        exit(EXIT_FAILURE);
    }

    const std::filesystem::path input_file_path = argv[1];
    const std::string input_filename = input_file_path.stem();
    const std::filesystem::path input_dir = input_file_path.root_directory();

    std::vector<Token> file_tokens = tokenize_file(input_file_path);
    for (const Token& token : file_tokens) {
        std::cout << token.value << "\t" << to_string(token.type) << std::endl;
    }

    std::cout << "SIZE: " << file_tokens.size() << std::endl;

    Parser parser(std::move(file_tokens));

    ASTNode expr_node = parser.parse_expr();

    print_ast(expr_node);

    std::vector<Token> tokens;
    tokens.push_back({ TokenType::u64, "99" });
    tokens.push_back({ TokenType::sub });
    tokens.push_back({ TokenType::u64, "10" });
    {
        std::fstream file((input_dir / (input_filename + ".asm")), std::ios::out);

        gen::print_u64_def(file);
        gen::start(file);
        {
            gen::ast_expr(file, expr_node);
            file << "    pop rdi\n";
            gen::print_u64(file);
            gen::print_newline(file);
            gen::exit(file);
        }

        //        gen::print_u64_def(file);
        //        gen::start(file);
        //        {
        //            gen::statement(file, tokens[0], tokens[1], tokens[2]);
        //            file << "    ;; -- moving for print ---\n";
        //            file << "    mov rdi, rax\n";
        //            gen::print_u64(file);
        //            gen::print_newline(file);
        //            gen::exit(file);
        //        }

        //        gen::print_u64_def(file);
        //        gen::start(file);
        //        {
        //            gen::print_u64(file, 777);
        //            gen::print_newline(file);
        //            gen::print_u64(file, 420);
        //            gen::print_newline(file);
        //            gen::exit(file);
        //        }
    }

    std::stringstream nasm_cmd;
    nasm_cmd << "nasm -felf64 " << input_dir / input_filename << ".asm";
    int return_val = system(nasm_cmd.str().c_str());

    if (return_val != 0) {
        std::cerr << "[Error] Assembling failed\n";
        exit(EXIT_FAILURE);
    }

    std::stringstream ld_cmd;
    ld_cmd << "ld " << input_dir / input_filename << ".o -o " << input_filename;
    return_val = system(ld_cmd.str().c_str());

    if (return_val != 0) {
        std::cerr << "[Error] Linking failed\n";
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}