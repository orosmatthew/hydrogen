#pragma once

#include "ast.hpp"

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

    void ast_factor(std::fstream& file, const ast::NodeFactor* factor)
    {
        if (auto* node_paren = std::get_if<ast::NodeFactorParen*>(&factor->var)) {
            ast_expr(file, (*node_paren)->expr);
        }
        else if (auto node_pos = std::get_if<ast::NodeFactorPos*>(&factor->var)) {
            push(file, (*node_pos)->tok_num->value);
        }
        else if (auto node_neg = std::get_if<ast::NodeFactorNeg*>(&factor->var)) {
            file << "    mov rcx, " << std::stoul((*node_neg)->tok_num->value) << "\n";
            file << "    neg rcx\n";
            push(file, "rcx");
        }
        else if (auto node_ident = std::get_if<ast::NodeFactorIdent*>(&factor->var)) {
            if (!m_vars.contains((*node_ident)->tok_ident->value)) {
                std::cerr << "[Error] Undefined identifier" << std::endl;
                ::exit(EXIT_FAILURE);
            }
            std::stringstream str;
            str << "QWORD [rsp + 8*" << m_stack_loc - m_vars.at((*node_ident)->tok_ident->value) << "]";
            push(file, str.str());
        }
        else {
            std::cerr << "[Error] Unreachable" << std::endl;
            ::exit(EXIT_FAILURE);
        }
    }

    void ast_expr_pred(std::fstream& file, const ast::NodeExprPred* expr_pred)
    {
        if (auto node_add = std::get_if<ast::NodeExprPredAdd*>(&expr_pred->var)) {
            ast_term(file, (*node_add)->term);
            pop(file, "rcx");
            pop(file, "rdx");
            file << "    add rdx, rcx\n";
            push(file, "rdx");
            if ((*node_add)->expr_pred.has_value()) {
                ast_expr_pred(file, (*node_add)->expr_pred.value());
            }
        }
        else if (auto node_sub = std::get_if<ast::NodeExprPredSub*>(&expr_pred->var)) {
            ast_term(file, (*node_sub)->term);
            pop(file, "rcx");
            pop(file, "rdx");
            file << "    sub rdx, rcx\n";
            push(file, "rdx");
            if ((*node_sub)->expr_pred.has_value()) {
                ast_expr_pred(file, (*node_sub)->expr_pred.value());
            }
        }
    }

    void ast_term_pred(std::fstream& file, const ast::NodeTermPred* term_pred)
    {
        if (auto node_multi = std::get_if<ast::NodeTermPredMulti*>(&term_pred->var)) {
            ast_factor(file, (*node_multi)->factor);
            pop(file, "rax");
            pop(file, "rcx");
            file << "    imul rcx\n";
            push(file, "rax");
            if ((*node_multi)->term_pred.has_value()) {
                ast_term_pred(file, (*node_multi)->term_pred.value());
            }
        }
        else if (auto node_div = std::get_if<ast::NodeTermPredDiv*>(&term_pred->var)) {
            ast_factor(file, (*node_div)->factor);
            pop(file, "rcx");
            pop(file, "rax");
            file << "    cqo\n";
            file << "    idiv rcx\n";
            push(file, "rax");
            if ((*node_div)->term_pred.has_value()) {
                ast_term_pred(file, (*node_div)->term_pred.value());
            }
        }
    }

    void ast_term(std::fstream& file, const ast::NodeTerm* term)
    {
        ast_factor(file, term->factor);
        if (term->term_pred.has_value()) {
            ast_term_pred(file, term->term_pred.value());
        }
    }

    void ast_expr(std::fstream& file, const ast::NodeExpr* expr)
    {
        ast_term(file, expr->term);
        if (expr->expr_pred.has_value()) {
            ast_expr_pred(file, expr->expr_pred.value());
        }
    }

    void ast_stmt_pred(std::fstream& file, const ast::NodeStmtPred* stmt_pred)
    {
        ast_stmt(file, stmt_pred->stmt);
    }

    void ast_let(std::fstream& file, const ast::NodeLet* let)
    {
        if (!m_vars.contains(let->tok_ident->value)) {
            ast_expr(file, let->expr);
            m_vars.insert({ let->tok_ident->value, m_stack_loc });
        }
        else {
            std::cerr << "[Error] Identifier already defined" << std::endl;
            ::exit(EXIT_FAILURE);
        }
    }

    void ast_stmt(std::fstream& file, const ast::NodeStmt* stmt)
    {
        file << "    ;; -- stmt --\n";
        if (auto stmt_expr = std::get_if<ast::NodeStmtExpr*>(&stmt->var)) {
            ast_expr(file, (*stmt_expr)->expr);
            pop(file, "rdi");
            print_i64(file);
            print_newline(file);
            if ((*stmt_expr)->stmt_pred.has_value()) {
                ast_stmt_pred(file, (*stmt_expr)->stmt_pred.value());
            }
        }
        else if (auto stmt_let = std::get_if<ast::NodeStmtLet*>(&stmt->var)) {
            ast_let(file, (*stmt_let)->let);
            if ((*stmt_let)->stmt_pred.has_value()) {
                ast_stmt_pred(file, (*stmt_let)->stmt_pred.value());
            }
        }
        else {
            std::cerr << "[Error] Unreachable" << std::endl;
            ::exit(EXIT_FAILURE);
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
