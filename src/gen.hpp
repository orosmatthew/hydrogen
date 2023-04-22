#pragma once

#include <cassert>
#include <stack>

#include "ast.hpp"
#include "token.hpp"

class Generator {
public:
    enum class PrimitiveType { i32, u64, i64 };

    explicit Generator(std::fstream& file)
        : m_file(file)
        , m_stack_loc(0)
        , m_label_count(0)
        , m_data_count(0)
    {
        m_data_stream << "section .data\n";
    }

    void print_u64_def()
    {
        m_file << ";; -- print_u64_def --\n";
        m_file << "print_u64:\n";
        m_file << "    sub rsp, 40\n";
        m_file << "    xor ecx, ecx\n";
        m_file << "    mov rsi, -3689348814741910323\n";
        m_file << ".LBB0_1:\n";
        m_file << "    mov rax, rdi\n";
        m_file << "    mul rsi\n";
        m_file << "    shr rdx, 3\n";
        m_file << "    lea eax, [rdx + rdx]\n";
        m_file << "    lea eax, [rax + 4*rax]\n";
        m_file << "    mov r8d, edi\n";
        m_file << "    sub r8d, eax\n";
        m_file << "    or  r8b, 48\n";
        m_file << "    mov byte [rsp + rcx + 31], r8b\n";
        m_file << "    dec rcx\n";
        m_file << "    cmp rdi, 9\n";
        m_file << "    mov rdi, rdx\n";
        m_file << "    ja  .LBB0_1\n";
        m_file << "    lea rsi, [rsp + rcx]\n";
        m_file << "    add rsi, 32\n";
        m_file << "    neg rcx\n";
        m_file << "    mov edi, 1\n";
        m_file << "    mov rdx, rcx\n";
        m_file << "    mov rax, 1\n";
        m_file << "    syscall\n";
        m_file << "    add rsp, 40\n";
        m_file << "    ret\n";
    }

    void print_i64_def()
    {
        m_file << "print_i64:"
                  "        sub     rsp, 40\n"
                  "        mov     rsi, rdi\n"
                  "        neg     rsi\n"
                  "        cmovs   rsi, rdi\n"
                  "        xor     ecx, ecx\n"
                  "        mov  r8, -3689348814741910323\n"
                  ".LBB1_1:\n"
                  "        mov     rax, rsi\n"
                  "        mul     r8\n"
                  "        shr     rdx, 3\n"
                  "        lea     eax, [rdx + rdx]\n"
                  "        lea     eax, [rax + 4*rax]\n"
                  "        mov     r9d, esi\n"
                  "        sub     r9d, eax\n"
                  "        or      r9b, 48\n"
                  "        mov     byte [rsp + rcx + 31], r9b\n"
                  "        dec     rcx\n"
                  "        cmp     rsi, 9\n"
                  "        mov     rsi, rdx\n"
                  "        ja      .LBB1_1\n"
                  "        test    rdi, rdi\n"
                  "        js      .LBB1_4\n"
                  "        neg     rcx\n"
                  "        jmp     .LBB1_5\n"
                  ".LBB1_4:\n"
                  "        mov     byte [rsp + rcx + 31], 45\n"
                  "        mov     eax, 1\n"
                  "        sub     rax, rcx\n"
                  "        mov     rcx, rax\n"
                  ".LBB1_5:\n"
                  "        mov     rsi, rsp\n"
                  "        sub     rsi, rcx\n"
                  "        add     rsi, 32\n"
                  "        mov     edi, 1\n"
                  "        mov     rdx, rcx\n"
                  "        mov     rax, 1\n"
                  "        syscall\n"
                  "        add     rsp, 40\n"
                  "        ret\n";
    }

    void print_u64()
    {
        m_file << "    call print_u64\n";
    }

    void print_i64()
    {
        m_file << "    ;; -- print_i64 --\n";
        m_file << "    call print_i64\n";
    }
    void print_newline()
    {
        m_file << "    ;; -- print_newline --\n";
        push("0xA");
        m_file << "    mov rax, 1\n";
        m_file << "    mov rdi, 1\n";
        m_file << "    mov rsi, rsp\n";
        m_file << "    mov rdx, 1\n";
        m_file << "    syscall\n";
        m_file << "    add rsp, 8\n";
        m_stack_loc--;
    }
    void start()
    {
        m_file << ";; -- start --\n";
        m_file << "section .text\n";
        m_file << "global _start\n";
        m_file << "_start:\n";
    }

    void exit()
    {
        m_file << "    ;; -- exit --\n";
        m_file << "    mov rax, 0x3c\n";
        m_file << "    mov rdi, 0\n";
        m_file << "    syscall\n";
    }

    void append_data()
    {
        m_file << m_data_stream.str();
    }

    PrimitiveType ast_expr_bin(const ast::NodeExprBin* expr_bin)
    {
        PrimitiveType type1 = ast_expr(expr_bin->lhs);
        PrimitiveType type2 = ast_expr(expr_bin->rhs);
        if (type1 != type2) {
            std::cerr << "[Error] Incompatible types" << std::endl;
            ::exit(EXIT_FAILURE);
        }
        if (expr_bin->tok_op->type == TokenType::multi) {
            pop("rax");
            pop("rbx");
            m_file << "    imul rbx\n";
            push("rax");
        }
        else if (expr_bin->tok_op->type == TokenType::div) {
            pop("rbx");
            pop("rax");
            m_file << "    cqo\n";
            m_file << "    idiv rbx\n";
            push("rax");
        }
        else if (expr_bin->tok_op->type == TokenType::add) {
            pop("rax");
            pop("rbx");
            m_file << "    add rax, rbx\n";
            push("rax");
        }
        else if (expr_bin->tok_op->type == TokenType::sub) {
            pop("rcx");
            pop("rdx");
            m_file << "    sub rdx, rcx\n";
            push("rdx");
        }
        else if (expr_bin->tok_op->type == TokenType::lt) {
            pop("rax");
            pop("rbx");
            m_file << "    cmp rax, rbx\n";
            m_file << "    setg al\n";
            m_file << "    movzx rax, al\n";
            push("rax");
        }
        else if (expr_bin->tok_op->type == TokenType::gt) {
            pop("rax");
            pop("rbx");
            m_file << "    cmp rax, rbx\n";
            m_file << "    setl al\n";
            m_file << "    movzx rax, al\n";
            push("rax");
        }
        else if (expr_bin->tok_op->type == TokenType::lte) {
            pop("rax");
            pop("rbx");
            m_file << "    cmp rax, rbx\n";
            m_file << "    setge al\n";
            m_file << "    movzx rax, al\n";
            push("rax");
        }
        else if (expr_bin->tok_op->type == TokenType::gte) {
            pop("rax");
            pop("rbx");
            m_file << "    cmp rax, rbx\n";
            m_file << "    setle al\n";
            m_file << "    movzx rax, al\n";
            push("rax");
        }
        else if (expr_bin->tok_op->type == TokenType::deq) {
            pop("rax");
            pop("rbx");
            m_file << "    cmp rax, rbx\n";
            m_file << "    sete al\n";
            m_file << "    movzx rax, al\n";
            push("rax");
        }
        else if (expr_bin->tok_op->type == TokenType::neq) {
            pop("rax");
            pop("rbx");
            m_file << "    cmp rax, rbx\n";
            m_file << "    setne al\n";
            m_file << "    movzx rax, al\n";
            push("rax");
        }
        else {
            // Unreachable
            assert(false);
        }
        return type1;
    }

    PrimitiveType ast_term_base(const ast::NodeTermBase* term_base)
    {
        if (auto* node_paren = std::get_if<ast::NodeTermBaseParen*>(&term_base->var)) {
            return ast_expr((*node_paren)->expr);
        }
        else if (auto node_num = std::get_if<ast::NodeTermBaseNum*>(&term_base->var)) {
            push((*node_num)->tok_num->value);
            return PrimitiveType::i64;
        }
        else if (auto node_ident = std::get_if<ast::NodeTermBaseIdent*>(&term_base->var)) {
            if (!m_vars_lookup.contains((*node_ident)->tok_ident->value)) {
                std::cerr << "[Error] Undefined identifier" << std::endl;
                ::exit(EXIT_FAILURE);
            }
            std::stringstream str;
            str << "QWORD [rsp + 8*" << m_stack_loc - m_vars_lookup.at((*node_ident)->tok_ident->value)->stack_offset
                << "]";
            push(str.str());
            return m_vars_lookup.at((*node_ident)->tok_ident->value)->type;
        }
        else if (auto term_base_str = std::get_if<ast::NodeTermBaseStr*>(&term_base->var)) {
            int data_num = m_data_count++;
            m_data_stream << "    D" << data_num << ": db \"";
            for (auto c : (*term_base_str)->tok_str_lit->value) {
                if (c == '\n') {
                    m_data_stream << "\", 0Ah, \"";
                }
                else {
                    m_data_stream << c;
                }
            }
            m_data_stream << "\"\n";
            push("D" + std::to_string(data_num));
            return PrimitiveType::u64;
        }
        else {
            // Unreachable
            assert(false);
        }
    }

    PrimitiveType ast_term(const ast::NodeTerm* term)
    {
        if (auto node_neg = std::get_if<ast::NodeTermNeg*>(&term->var)) {
            PrimitiveType type = ast_term_base((*node_neg)->term_base);
            pop("rax");
            m_file << "    neg rax\n";
            push("rax");
            return type;
        }
        else if (auto node_base = std::get_if<ast::NodeTermBase*>(&term->var)) {
            return ast_term_base((*node_base));
        }
        else {
            // Unreachable
            assert(false);
        }
    }

    PrimitiveType ast_expr(const ast::NodeExpr* expr)
    {
        if (auto term = std::get_if<ast::NodeTerm*>(&expr->var)) {
            return ast_term((*term));
        }
        else if (auto expr_bin = std::get_if<ast::NodeExprBin*>(&expr->var)) {
            return ast_expr_bin((*expr_bin));
        }
        else {
            // Unreachable
            assert(false);
        }
    }

    void ast_scope(const ast::NodeScope* scope)
    {
        begin_scope();
        if (scope->stmt.has_value()) {
            ast_stmt(scope->stmt.value());
        }
        end_scope();
    }

    //    void ast_e2quation(const ast::NodeEq* eq)
    //    {
    //        if (auto eq_expr = std::get_if<ast::NodeEqExpr*>(&eq->var)) {
    //            ast_expr((*eq_expr)->expr);
    //        }
    //        else if (auto eq_str = std::get_if<ast::NodeEqStr*>(&eq->var)) {
    //            int data_num = m_data_count++;
    //            m_data_stream << "    D" << data_num << ": db \"";
    //            for (auto c : (*eq_str)->tok_str->value) {
    //                if (c == '\n') {
    //                    m_data_stream << "\", 0Ah, \"";
    //                }
    //                else {
    //                    m_data_stream << c;
    //                }
    //            }
    //            m_data_stream << "\"\n";
    //            push("D" + std::to_string(data_num));
    //        }
    //        else {
    //            // Unreachable
    //            assert(false);
    //        }
    //    }

    void ast_stmt(const ast::NodeStmt* stmt)
    {
        m_file << "    ;; -- stmt --\n";
        if (auto stmt_print = std::get_if<ast::NodeStmtPrint*>(&stmt->var)) {
            ast_expr((*stmt_print)->expr);
            pop("rdi");
            print_i64();
            print_newline();
            if ((*stmt_print)->next_stmt.has_value()) {
                ast_stmt((*stmt_print)->next_stmt.value());
            }
        }
        else if (auto stmt_let = std::get_if<ast::NodeStmtLet*>(&stmt->var)) {
            if (!m_vars_lookup.contains((*stmt_let)->tok_ident->value)) {
                PrimitiveType type = ast_expr((*stmt_let)->expr);
                push_var((*stmt_let)->tok_ident->value, type);
            }
            else {
                std::cerr << "[Error] Identifier already defined" << std::endl;
                ::exit(EXIT_FAILURE);
            }
            if ((*stmt_let)->next_stmt.has_value()) {
                ast_stmt((*stmt_let)->next_stmt.value());
            }
        }
        else if (auto stmt_eq = std::get_if<ast::NodeStmtEq*>(&stmt->var)) {
            if (!m_vars_lookup.contains((*stmt_eq)->tok_ident->value)) {
                std::cerr << "[Error] Unknown identifier" << std::endl;
                ::exit(EXIT_FAILURE);
            }
            PrimitiveType type = ast_expr((*stmt_eq)->expr);
            if (type != m_vars_lookup.at((*stmt_eq)->tok_ident->value)->type) {
                std::cerr << "[Error] Invalid type" << std::endl;
                ::exit(EXIT_FAILURE);
            }
            pop("rax");
            m_file << "    mov QWORD [rsp + 8*"
                   << m_stack_loc - m_vars_lookup.at((*stmt_eq)->tok_ident->value)->stack_offset << "], rax\n";
            if ((*stmt_eq)->next_stmt.has_value()) {
                ast_stmt((*stmt_eq)->next_stmt.value());
            }
        }
        else if (auto stmt_if = std::get_if<ast::NodeStmtIf*>(&stmt->var)) {
            ast_expr((*stmt_if)->expr);
            const std::string else_label = get_next_label();
            pop("rax");
            m_file << "    test rax, rax\n";
            m_file << "    jz " << else_label << "\n";
            ast_scope((*stmt_if)->scope);
            if ((*stmt_if)->else_.has_value()) {
                const std::string end_label = get_next_label();
                m_file << "    jmp " << end_label << "\n";
                m_file << else_label << ":\n";
                ast_scope((*stmt_if)->else_.value()->scope);
                m_file << end_label << ":\n";
            }
            else {
                m_file << else_label << ":\n";
            }
            if ((*stmt_if)->next_stmt.has_value()) {
                ast_stmt((*stmt_if)->next_stmt.value());
            }
        }
        else if (auto stmt_scope = std::get_if<ast::NodeStmtScope*>(&stmt->var)) {
            ast_scope((*stmt_scope)->scope);
            if ((*stmt_scope)->next_stmt.has_value()) {
                ast_stmt((*stmt_scope)->next_stmt.value());
            }
        }
        else if (auto stmt_write = std::get_if<ast::NodeStmtWrite*>(&stmt->var)) {
            ast_expr((*stmt_write)->expr1);
            ast_expr((*stmt_write)->expr2);
            pop("rdx");
            pop("rsi");
            m_file << "    mov rax, 1\n";
            m_file << "    mov rdi, 1\n";
            m_file << "    syscall\n";
            if ((*stmt_write)->next_stmt.has_value()) {
                ast_stmt((*stmt_write)->next_stmt.value());
            }
        }
        else {
            // Unreachable
            assert(false);
        }
    }

    void push(const std::string& str)
    {
        m_file << "    push " << str << "\n";
        m_stack_loc++;
    }

    void pop(const std::string& str)
    {
        m_file << "    pop " << str << "\n";
        m_stack_loc--;
    }

    std::string get_next_label()
    {
        return ".L" + std::to_string(m_label_count++);
    }

    void begin_scope()
    {
        m_scopes.push(m_vars.size());
    }

    void end_scope()
    {
        int var_pop_count = 0;
        while (m_vars.size() > m_scopes.top()) {
            m_vars_lookup.erase(m_vars.back().name);
            m_vars.pop_back();
            var_pop_count++;
        }
        m_file << "    add rsp, " << 8 * var_pop_count << "\n";
        m_stack_loc -= var_pop_count;
    }

    void push_var(const std::string& name, PrimitiveType type)
    {
        assert(!m_vars_lookup.contains(name));
        m_vars.push_back({ .name = name, .stack_offset = m_stack_loc, .type = type });
        m_vars_lookup.insert({ name, &m_vars.back() });
    }

private:
    struct Var {
        std::string name;
        int stack_offset;
        PrimitiveType type;
    };

    std::fstream& m_file;
    int m_stack_loc;
    std::vector<Var> m_vars {};
    std::unordered_map<std::string, Var*> m_vars_lookup {};
    std::stack<size_t> m_scopes;
    int m_label_count;
    int m_data_count;
    std::stringstream m_data_stream;
};
