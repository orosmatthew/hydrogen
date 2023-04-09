#pragma once

#include "ast.hpp"
#include "token.hpp"
#include <cassert>

class Generator {
public:
    explicit Generator(std::fstream& file)
        : m_file(file)
        , m_stack_loc(0)
        , m_label_count(0)
    {
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

    void ast_expr_bin(const ast::NodeExprBin* expr_bin)
    {
        ast_expr(expr_bin->lhs);
        ast_expr(expr_bin->rhs);
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
    }

    void ast_term_base(const ast::NodeTermBase* term_base)
    {
        if (auto* node_paren = std::get_if<ast::NodeTermBaseParen*>(&term_base->var)) {
            ast_expr((*node_paren)->expr);
        }
        else if (auto node_num = std::get_if<ast::NodeTermBaseNum*>(&term_base->var)) {
            push((*node_num)->tok_num->value);
        }
        else if (auto node_ident = std::get_if<ast::NodeTermBaseIdent*>(&term_base->var)) {
            if (!m_vars.contains((*node_ident)->tok_ident->value)) {
                std::cerr << "[Error] Undefined identifier" << std::endl;
                ::exit(EXIT_FAILURE);
            }
            std::stringstream str;
            str << "QWORD [rsp + 8*" << m_stack_loc - m_vars.at((*node_ident)->tok_ident->value) << "]";
            push(str.str());
        }
        else {
            // Unreachable
            assert(false);
        }
    }

    void ast_term(const ast::NodeTerm* term)
    {
        if (auto node_neg = std::get_if<ast::NodeTermNeg*>(&term->var)) {
            ast_term_base((*node_neg)->term_base);
            pop("rax");
            m_file << "    neg rax\n";
            push("rax");
        }
        else if (auto node_base = std::get_if<ast::NodeTermBase*>(&term->var)) {
            ast_term_base((*node_base));
        }
        else {
            // Unreachable
            assert(false);
        }
    }

    void ast_expr(const ast::NodeExpr* expr)
    {
        if (auto term = std::get_if<ast::NodeTerm*>(&expr->var)) {
            ast_term((*term));
        }
        else if (auto expr_bin = std::get_if<ast::NodeExprBin*>(&expr->var)) {
            ast_expr_bin((*expr_bin));
        }
        else {
            // Unreachable
            assert(false);
        }
    }

    void ast_stmt_pred(const ast::NodeStmtPred* stmt_pred)
    {
        ast_stmt(stmt_pred->stmt);
    }

    void ast_stmt(const ast::NodeStmt* stmt)
    {
        m_file << "    ;; -- stmt --\n";
        if (auto stmt_print = std::get_if<ast::NodeStmtPrint*>(&stmt->var)) {
            ast_expr((*stmt_print)->expr);
            pop("rdi");
            print_i64();
            print_newline();
            if ((*stmt_print)->stmt_pred.has_value()) {
                ast_stmt_pred((*stmt_print)->stmt_pred.value());
            }
        }
        else if (auto stmt_let = std::get_if<ast::NodeStmtLet*>(&stmt->var)) {
            if (!m_vars.contains((*stmt_let)->tok_ident->value)) {
                ast_expr((*stmt_let)->expr);
                m_vars.insert({ (*stmt_let)->tok_ident->value, m_stack_loc });
            }
            else {
                std::cerr << "[Error] Identifier already defined" << std::endl;
                ::exit(EXIT_FAILURE);
            }
            if ((*stmt_let)->stmt_pred.has_value()) {
                ast_stmt_pred((*stmt_let)->stmt_pred.value());
            }
        }
        else if (auto stmt_eq = std::get_if<ast::NodeStmtEq*>(&stmt->var)) {
            if (!m_vars.contains((*stmt_eq)->tok_ident->value)) {
                std::cerr << "[Error] Unknown identifier" << std::endl;
                ::exit(EXIT_FAILURE);
            }
            ast_expr((*stmt_eq)->expr);
            pop("rax");
            m_file << "    mov QWORD [rsp + 8*" << m_stack_loc - m_vars.at((*stmt_eq)->tok_ident->value) << "], rax\n";
            if ((*stmt_eq)->stmt_pred.has_value()) {
                ast_stmt_pred((*stmt_eq)->stmt_pred.value());
            }
        }
        else if (auto stmt_if = std::get_if<ast::NodeStmtIf*>(&stmt->var)) {
            ast_expr((*stmt_if)->expr);
            const std::string else_label = get_next_label();
            pop("rax");
            m_file << "    test rax, rax\n";
            m_file << "    jz " << else_label << "\n";
            ast_stmt((*stmt_if)->stmt);
            if ((*stmt_if)->else_.has_value()) {
                const std::string end_label = get_next_label();
                m_file << "    jmp " << end_label << "\n";
                m_file << else_label << ":\n";
                ast_stmt((*stmt_if)->else_.value()->stmt);
                m_file << end_label << ":\n";
            }
            else {
                m_file << else_label << ":\n";
            }
            if ((*stmt_if)->stmt_pred.has_value()) {
                ast_stmt_pred((*stmt_if)->stmt_pred.value());
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

private:
    std::fstream& m_file;
    int m_stack_loc;
    std::unordered_map<std::string, int> m_vars {};
    int m_label_count;
};
