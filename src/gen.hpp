#pragma once

#include "ast.hpp"
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

void print_i64_def(std::fstream& file)
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

void print_u64(std::fstream& file)
{
    file << "    call print_u64\n";
}

void print_i64(std::fstream& file)
{
    file << "    ;; -- print_i64 --\n";
    file << "    call print_i64\n";
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

void ast_expr(std::fstream& file, const ast::NodeExpr* expr);

void ast_factor(std::fstream& file, const ast::NodeFactor* factor)
{
    if (auto* node_paren = std::get_if<ast::NodeFactorParen*>(&factor->var)) {
        ast_expr(file, (*node_paren)->expr);
    }
    else if (auto node_pos = std::get_if<ast::NodeFactorPos*>(&factor->var)) {
        file << "    push " << std::stoul((*node_pos)->tok_num->value) << "\n";
    }
    else if (auto node_neg = std::get_if<ast::NodeFactorNeg*>(&factor->var)) {
        file << "    mov rcx, " << std::stoul((*node_neg)->tok_num->value) << "\n";
        file << "    neg rcx\n";
        file << "    push rcx\n";
    }
}

void ast_term(std::fstream& file, const ast::NodeTerm* term);

void ast_expr_pred(std::fstream& file, const ast::NodeExprPred* expr_pred)
{
    if (!expr_pred->var.has_value()) {
        return;
    }
    if (auto node_add = std::get_if<ast::NodeExprPredAdd*>(&expr_pred->var.value())) {
        ast_term(file, (*node_add)->term);
        file << "    pop rcx\n";
        file << "    pop rdx\n";
        file << "    add rdx, rcx\n";
        file << "    push rdx\n";
        ast_expr_pred(file, (*node_add)->expr_pred);
    }
    else if (auto node_sub = std::get_if<ast::NodeExprPredSub*>(&expr_pred->var.value())) {
        ast_term(file, (*node_sub)->term);
        file << "    pop rcx\n";
        file << "    pop rdx\n";
        file << "    sub rdx, rcx\n";
        file << "    push rdx\n";
        ast_expr_pred(file, (*node_sub)->expr_pred);
    }
}

void ast_term_pred(std::fstream& file, const ast::NodeTermPred* term_pred)
{
    if (!term_pred->var.has_value()) {
        return;
    }
    if (auto node_multi = std::get_if<ast::NodeTermPredMulti*>(&term_pred->var.value())) {
        ast_factor(file, (*node_multi)->factor);
        file << "    pop rax\n";
        file << "    pop rcx\n";
        file << "    imul rcx\n";
        file << "    push rax\n";
        ast_term_pred(file, (*node_multi)->term_pred);
    }
    else if (auto node_div = std::get_if<ast::NodeTermPredDiv*>(&term_pred->var.value())) {
        ast_factor(file, (*node_div)->factor);
        file << "    pop rcx\n";
        file << "    pop rax\n";
        file << "    cqo\n";
        file << "    idiv rcx\n";
        file << "    push rax\n";
        ast_term_pred(file, (*node_div)->term_pred);
    }
}

void ast_term(std::fstream& file, const ast::NodeTerm* term)
{
    ast_factor(file, term->factor);
    ast_term_pred(file, term->term_pred);
}

void ast_expr(std::fstream& file, const ast::NodeExpr* expr)
{
    ast_term(file, expr->term);
    ast_expr_pred(file, expr->expr_pred);
}

void ast_stmt_pred(std::fstream& file, const ast::NodeStmtPred* stmt_pred)
{
    if (stmt_pred->var.has_value()) {
        file << "    ;; -- stmt --\n";
        ast_expr(file, stmt_pred->var.value()->expr);
        file << "    pop rdi\n";
        gen::print_i64(file);
        gen::print_newline(file);
        if (stmt_pred->var.value()->stmt_pred->var.has_value()) {
            ast_stmt_pred(file, stmt_pred->var.value()->stmt_pred->var.value()->stmt_pred);
        }
    }
}

void ast_stmt(std::fstream& file, const ast::NodeStmt* stmt)
{
    file << "    ;; -- stmt --\n";
    ast_expr(file, stmt->expr);
    file << "    pop rdi\n";
    gen::print_i64(file);
    gen::print_newline(file);
    ast_stmt_pred(file, stmt->stmt_pred);
}
} // namespace gen
