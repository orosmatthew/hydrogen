#pragma once

#include "ast.hpp"
#include "token.hpp"
#include <cassert>

class Generator {
public:
    Generator()
        : m_stack_loc(0)
    {
    }

    static void print_u64_def(std::fstream& file)
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

    static void print_i64_def(std::fstream& file)
    {
        file << "print_i64:"
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

    static void print_u64(std::fstream& file)
    {
        file << "    call print_u64\n";
    }

    static void print_i64(std::fstream& file)
    {
        file << "    ;; -- print_i64 --\n";
        file << "    call print_i64\n";
    }
    void print_newline(std::fstream& file)
    {
        file << "    ;; -- print_newline --\n";
        push(file, "0xA");
        file << "    mov rax, 1\n";
        file << "    mov rdi, 1\n";
        file << "    mov rsi, rsp\n";
        file << "    mov rdx, 1\n";
        file << "    syscall\n";
        file << "    add rsp, 8\n";
        m_stack_loc--;
    }

    static void start(std::fstream& file)
    {
        file << ";; -- start --\n";
        file << "section .text\n";
        file << "global _start\n";
        file << "_start:\n";
    }

    static void exit(std::fstream& file)
    {
        file << "    ;; -- exit --\n";
        file << "    mov rax, 0x3c\n";
        file << "    mov rdi, 0\n";
        file << "    syscall\n";
    }

    void ast_expr_bin(std::fstream& file, const ast::NodeExprBin* expr_bin)
    {
        ast_expr(file, expr_bin->lhs);
        ast_expr(file, expr_bin->rhs);
        if (expr_bin->tok_op->type == TokenType::multi) {
            pop(file, "rax");
            pop(file, "rbx");
            file << "    imul rbx\n";
            push(file, "rax");
        }
        else if (expr_bin->tok_op->type == TokenType::div) {
            pop(file, "rbx");
            pop(file, "rax");
            file << "    cqo\n";
            file << "    idiv rbx\n";
            push(file, "rax");
        }
        else if (expr_bin->tok_op->type == TokenType::add) {
            pop(file, "rax");
            pop(file, "rbx");
            file << "    add rax, rbx\n";
            push(file, "rax");
        }
        else if (expr_bin->tok_op->type == TokenType::sub) {
            pop(file, "rcx");
            pop(file, "rdx");
            file << "    sub rdx, rcx\n";
            push(file, "rdx");
        }
        else if (expr_bin->tok_op->type == TokenType::lt) {
            pop(file, "rax");
            pop(file, "rbx");
            file << "    cmp rax, rbx\n";
            file << "    setg al\n";
            file << "    movzx rax, al\n";
            push(file, "rax");
        }
        else if (expr_bin->tok_op->type == TokenType::gt) {
            pop(file, "rax");
            pop(file, "rbx");
            file << "    cmp rax, rbx\n";
            file << "    setl al\n";
            file << "    movzx rax, al\n";
            push(file, "rax");
        }
        else if (expr_bin->tok_op->type == TokenType::lte) {
            pop(file, "rax");
            pop(file, "rbx");
            file << "    cmp rax, rbx\n";
            file << "    setge al\n";
            file << "    movzx rax, al\n";
            push(file, "rax");
        }
        else if (expr_bin->tok_op->type == TokenType::gte) {
            pop(file, "rax");
            pop(file, "rbx");
            file << "    cmp rax, rbx\n";
            file << "    setle al\n";
            file << "    movzx rax, al\n";
            push(file, "rax");
        }
        else {
            // Unreachable
            assert(false);
        }
    }

    void ast_term_base(std::fstream& file, const ast::NodeTermBase* term_base)
    {
        if (auto* node_paren = std::get_if<ast::NodeTermBaseParen*>(&term_base->var)) {
            ast_expr(file, (*node_paren)->expr);
        }
        else if (auto node_num = std::get_if<ast::NodeTermBaseNum*>(&term_base->var)) {
            push(file, (*node_num)->tok_num->value);
        }
        else if (auto node_ident = std::get_if<ast::NodeTermBaseIdent*>(&term_base->var)) {
            if (!m_vars.contains((*node_ident)->tok_ident->value)) {
                std::cerr << "[Error] Undefined identifier" << std::endl;
                ::exit(EXIT_FAILURE);
            }
            std::stringstream str;
            str << "QWORD [rsp + 8*" << m_stack_loc - m_vars.at((*node_ident)->tok_ident->value) << "]";
            push(file, str.str());
        }
        else {
            // Unreachable
            assert(false);
        }
    }

    void ast_term(std::fstream& file, const ast::NodeTerm* term)
    {
        if (auto node_neg = std::get_if<ast::NodeTermNeg*>(&term->var)) {
            ast_term_base(file, (*node_neg)->term_base);
            pop(file, "rax");
            file << "    neg rax\n";
            push(file, "rax");
        }
        else if (auto node_base = std::get_if<ast::NodeTermBase*>(&term->var)) {
            ast_term_base(file, (*node_base));
        }
        else {
            // Unreachable
            assert(false);
        }
    }

    void ast_expr(std::fstream& file, const ast::NodeExpr* expr)
    {
        if (auto term = std::get_if<ast::NodeTerm*>(&expr->var)) {
            ast_term(file, (*term));
        }
        else if (auto expr_bin = std::get_if<ast::NodeExprBin*>(&expr->var)) {
            ast_expr_bin(file, (*expr_bin));
        }
        else {
            // Unreachable
            assert(false);
        }
    }

    void ast_stmt_pred(std::fstream& file, const ast::NodeStmtPred* stmt_pred)
    {
        ast_stmt(file, stmt_pred->stmt);
    }

    void ast_stmt(std::fstream& file, const ast::NodeStmt* stmt)
    {
        file << "    ;; -- stmt --\n";
        if (auto stmt_print = std::get_if<ast::NodeStmtPrint*>(&stmt->var)) {
            ast_expr(file, (*stmt_print)->expr);
            pop(file, "rdi");
            print_i64(file);
            print_newline(file);
            if ((*stmt_print)->stmt_pred.has_value()) {
                ast_stmt_pred(file, (*stmt_print)->stmt_pred.value());
            }
        }
        else if (auto stmt_let = std::get_if<ast::NodeStmtLet*>(&stmt->var)) {
            if (!m_vars.contains((*stmt_let)->tok_ident->value)) {
                ast_expr(file, (*stmt_let)->expr);
                m_vars.insert({ (*stmt_let)->tok_ident->value, m_stack_loc });
            }
            else {
                std::cerr << "[Error] Identifier already defined" << std::endl;
                ::exit(EXIT_FAILURE);
            }
            if ((*stmt_let)->stmt_pred.has_value()) {
                ast_stmt_pred(file, (*stmt_let)->stmt_pred.value());
            }
        }
        else if (auto stmt_eq = std::get_if<ast::NodeStmtEq*>(&stmt->var)) {
            if (!m_vars.contains((*stmt_eq)->tok_ident->value)) {
                std::cerr << "[Error] Unknown identifier" << std::endl;
                ::exit(EXIT_FAILURE);
            }
            ast_expr(file, (*stmt_eq)->expr);
            pop(file, "rax");
            file << "    mov QWORD [rsp + 8*" << m_stack_loc - m_vars.at((*stmt_eq)->tok_ident->value) << "], rax\n";
            if ((*stmt_eq)->stmt_pred.has_value()) {
                ast_stmt_pred(file, (*stmt_eq)->stmt_pred.value());
            }
        }
        else {
            // Unreachable
            assert(false);
        }
    }

    void push(std::fstream& file, const std::string& str)
    {
        file << "    push " << str << "\n";
        m_stack_loc++;
    }

    void pop(std::fstream& file, const std::string& str)
    {
        file << "    pop " << str << "\n";
        m_stack_loc--;
    }

private:
    int m_stack_loc;
    std::unordered_map<std::string, int> m_vars {};
};
