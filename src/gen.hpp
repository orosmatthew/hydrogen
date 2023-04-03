#pragma once

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

void ast_expr(std::fstream& file, const ast::NodeExpr& expr);

void ast_factor(std::fstream& file, const ast::NodeFactor& factor)
{
    if (factor.expr().has_value()) {
        ast_expr(file, factor.expr().value());
    }
    else if (factor.pos_i64().has_value()) {
        file << "    push " << std::stoul(factor.pos_i64().value()->value) << "\n";
    }
    else {
        file << "    mov rcx, " << std::stoul(factor.neg_i64().value()->value) << "\n";
        file << "    neg rcx\n";
        file << "    push rcx\n";
    }
}

void ast_term(std::fstream& file, const ast::NodeTerm& term);

void ast_expr_pred(std::fstream& file, const ast::NodeExprPred& expr_pred)
{
    if (expr_pred.empty()) {
        return;
    }
    ast_term(file, expr_pred.term().value());
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
    ast_expr_pred(file, expr_pred.expr_pred().value());
}

void ast_term_pred(std::fstream& file, const ast::NodeTermPred& term_pred)
{
    if (term_pred.empty()) {
        return;
    }
    ast_factor(file, term_pred.factor().value());
    if (term_pred.children.at(0).token->type == TokenType::multi) {
        file << "    pop rax\n";
        file << "    pop rcx\n";
        file << "    imul rcx\n";
        file << "    push rax\n";
    }
    else if (term_pred.children.at(0).token->type == TokenType::div) {
        file << "    pop rcx\n";
        file << "    pop rax\n";
        file << "    cqo\n";
        file << "    idiv rcx\n";
        file << "    push rax\n";
    }
    ast_term_pred(file, term_pred.term_pred().value());
}

void ast_term(std::fstream& file, const ast::NodeTerm& term)
{
    ast_factor(file, term.factor());
    ast_term_pred(file, term.term_pred());
}

void ast_expr(std::fstream& file, const ast::NodeExpr& expr)
{
    ast_term(file, expr.term());
    ast_expr_pred(file, expr.expr_pred());
}
} // namespace gen
