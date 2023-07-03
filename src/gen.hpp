#pragma once

#include <cassert>
#include <functional>
#include <ranges>
#include <stack>
#include <unordered_map>
#include <utility>

#include "ast.hpp"
#include "token.hpp"

class Generator {
public:
    enum class PrimitiveType { unknown, i32, u64, i64, bool_ };
    enum class ScopeType { generic, loop };

    explicit Generator(std::fstream& file)
        : m_file(file)
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
        push(PrimitiveType::unknown, "0xA");
        m_file << "    mov rax, 1\n";
        m_file << "    mov rdi, 1\n";
        m_file << "    mov rsi, rsp\n";
        m_file << "    mov rdx, 1\n";
        m_file << "    syscall\n";
        pop();
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
            push(PrimitiveType::i64, "rax");
        }
        else if (expr_bin->tok_op->type == TokenType::div) {
            pop("rbx");
            pop("rax");
            m_file << "    cqo\n";
            m_file << "    idiv rbx\n";
            push(PrimitiveType::i64, "rax");
        }
        else if (expr_bin->tok_op->type == TokenType::add) {
            pop("rax");
            pop("rbx");
            m_file << "    add rax, rbx\n";
            push(PrimitiveType::i64, "rax");
        }
        else if (expr_bin->tok_op->type == TokenType::sub) {
            pop("rcx");
            pop("rdx");
            m_file << "    sub rdx, rcx\n";
            push(PrimitiveType::i64, "rdx");
        }
        else if (expr_bin->tok_op->type == TokenType::lt) {
            pop("rax");
            pop("rbx");
            m_file << "    cmp rax, rbx\n";
            m_file << "    setg al\n";
            m_file << "    movzx rax, al\n";
            push(PrimitiveType::i64, "rax");
        }
        else if (expr_bin->tok_op->type == TokenType::gt) {
            pop("rax");
            pop("rbx");
            m_file << "    cmp rax, rbx\n";
            m_file << "    setl al\n";
            m_file << "    movzx rax, al\n";
            push(PrimitiveType::i64, "rax");
        }
        else if (expr_bin->tok_op->type == TokenType::lte) {
            pop("rax");
            pop("rbx");
            m_file << "    cmp rax, rbx\n";
            m_file << "    setge al\n";
            m_file << "    movzx rax, al\n";
            push(PrimitiveType::i64, "rax");
        }
        else if (expr_bin->tok_op->type == TokenType::gte) {
            pop("rax");
            pop("rbx");
            m_file << "    cmp rax, rbx\n";
            m_file << "    setle al\n";
            m_file << "    movzx rax, al\n";
            push(PrimitiveType::i64, "rax");
        }
        else if (expr_bin->tok_op->type == TokenType::deq) {
            pop("rax");
            pop("rbx");
            m_file << "    cmp rax, rbx\n";
            m_file << "    sete al\n";
            m_file << "    movzx rax, al\n";
            push(PrimitiveType::i64, "rax");
        }
        else if (expr_bin->tok_op->type == TokenType::neq) {
            pop("rax");
            pop("rbx");
            m_file << "    cmp rax, rbx\n";
            m_file << "    setne al\n";
            m_file << "    movzx rax, al\n";
            push(PrimitiveType::i64, "rax");
        }
        else {
            // Unreachable
            assert(false);
        }
        return type1;
    }

    PrimitiveType ast_term_base(const ast::NodeTermBase* term_base)
    {
        struct TermBaseVisitor {
            Generator* gen = nullptr;
            PrimitiveType type = PrimitiveType::unknown;

            void operator()(ast::NodeTermBaseParen* term_base_paren)
            {
                type = gen->ast_expr(term_base_paren->expr);
            }
            void operator()(ast::NodeTermBaseNum* term_base_num)
            {
                gen->push(PrimitiveType::i64, term_base_num->tok_num->value);
                type = PrimitiveType::i64;
            }
            void operator()(ast::NodeTermBaseIdent* term_base_ident)
            {
                std::optional<StackItem> var = gen->stack_item_by_name(term_base_ident->tok_ident->value);
                if (!var.has_value()) {
                    std::cerr << "[Error] Undefined identifier" << std::endl;
                    ::exit(EXIT_FAILURE);
                }
                std::stringstream str;
                str << "QWORD [rsp + 8*" << var->stack_offset(gen->m_stack.size()) << "]";
                gen->push(PrimitiveType::i64, str.str());

                if (term_base_ident->post.has_value()) {
                    struct PostVisitor {
                        Generator* gen;
                        const StackItem* var;

                        void operator()(ast::NodePostInc* post_inc) const
                        {
                            if (var->type != PrimitiveType::i64) {
                                std::cerr << "[Error] Increment not valid for type" << std::endl;
                                ::exit(EXIT_FAILURE);
                            }
                            gen->m_file << "    mov rax, [rsp + 8*" << var->stack_offset(gen->m_stack.size()) << "]\n";
                            gen->m_file << "    mov rbx, 1\n";
                            gen->m_file << "    add rax, rbx\n";
                            gen->m_file << "    mov QWORD [rsp + 8*" << var->stack_offset(gen->m_stack.size())
                                        << "], rax\n";
                        }
                        void operator()(ast::NodePostDec* post_dec) const
                        {
                            if (var->type != PrimitiveType::i64) {
                                std::cerr << "[Error] Increment not valid for type" << std::endl;
                                ::exit(EXIT_FAILURE);
                            }
                            gen->m_file << "    mov rax, [rsp + 8*" << var->stack_offset(gen->m_stack.size()) << "]\n";
                            gen->m_file << "    mov rbx, 1\n";
                            gen->m_file << "    sub rax, rbx\n";
                            gen->m_file << "    mov QWORD [rsp + 8*" << var->stack_offset(gen->m_stack.size())
                                        << "], rax\n";
                        }
                    };
                    PostVisitor visitor(gen, &var.value());
                    std::visit(visitor, term_base_ident->post.value()->var);
                }

                type = gen->m_stack.at(gen->m_vars_lookup.at(term_base_ident->tok_ident->value)).type;
            }
            void operator()(ast::NodeTermBaseStr* term_base_str)
            {
                int data_num = gen->m_data_count++;
                gen->m_data_stream << "    D" << data_num << ": db \"";
                for (auto c : term_base_str->tok_str_lit->value) {
                    if (c == '\n') {
                        gen->m_data_stream << "\", 0Ah, \"";
                    }
                    else {
                        gen->m_data_stream << c;
                    }
                }
                gen->m_data_stream << "\"\n";
                gen->push(PrimitiveType::i64, "D" + std::to_string(data_num));
                type = PrimitiveType::u64;
            }
            void operator()(ast::NodeTermBaseTrue*)
            {
                gen->push(PrimitiveType::i64, "1");
                type = PrimitiveType::bool_;
            }
            void operator()(ast::NodeTermBaseFalse*)
            {
                gen->push(PrimitiveType::i64, "0");
                type = PrimitiveType::bool_;
            }
            void operator()(ast::NodeTermBaseInc* term_base_inc)
            {
                std::optional<StackItem> var = gen->stack_item_by_name(term_base_inc->tok_ident->value);
                if (!var.has_value()) {
                    std::cerr << "[Error] Undefined identifier" << std::endl;
                    ::exit(EXIT_FAILURE);
                }
                if (var->type != PrimitiveType::i64) {
                    std::cerr << "[Error] Cannot increment on type" << std::endl;
                    ::exit(EXIT_FAILURE);
                }
                gen->m_file << "    mov rax, QWORD [rsp + 8*" << var->stack_offset(gen->m_stack.size()) << "]\n";
                gen->m_file << "    mov rbx, 1\n";
                gen->m_file << "    add rax, rbx\n";
                gen->m_file << "    mov QWORD [rsp + 8*" << var->stack_offset(gen->m_stack.size()) << "], rax\n";
                gen->push(PrimitiveType::i64, "rax");
                type = PrimitiveType::i64;
            }
            void operator()(ast::NodeTermBaseDec* term_base_dec)
            {
                std::optional<StackItem> var = gen->stack_item_by_name(term_base_dec->tok_ident->value);
                if (!var.has_value()) {
                    std::cerr << "[Error] Undefined identifier" << std::endl;
                    ::exit(EXIT_FAILURE);
                }
                if (var->type != PrimitiveType::i64) {
                    std::cerr << "[Error] Cannot decrement on type" << std::endl;
                    ::exit(EXIT_FAILURE);
                }
                gen->m_file << "    mov rax, QWORD [rsp + 8*" << var->stack_offset(gen->m_stack.size()) << "]\n";
                gen->m_file << "    mov rbx, 1\n";
                gen->m_file << "    sub rax, rbx\n";
                gen->m_file << "    mov QWORD [rsp + 8*" << var->stack_offset(gen->m_stack.size()) << "], rax\n";
                gen->push(PrimitiveType::i64, "rax");
                type = PrimitiveType::i64;
            }
        };

        TermBaseVisitor visitor { this };
        std::visit(visitor, term_base->var);
        assert(visitor.type != PrimitiveType::unknown);
        return visitor.type;
    }

    PrimitiveType ast_term(const ast::NodeTerm* term)
    {
        struct TermVisitor {
            Generator* gen = nullptr;
            PrimitiveType type = PrimitiveType::unknown;

            void operator()(ast::NodeTermNeg* term_neg)
            {
                type = gen->ast_term_base(term_neg->term_base);
                gen->pop("rax");
                gen->m_file << "    neg rax\n";
                gen->push(PrimitiveType::i64, "rax");
            }
            void operator()(ast::NodeTermBase* term_base)
            {
                type = gen->ast_term_base(term_base);
            }
        };
        TermVisitor visitor { this };
        std::visit(visitor, term->var);
        assert(visitor.type != PrimitiveType::unknown);
        return visitor.type;
    }

    PrimitiveType ast_expr(const ast::NodeExpr* expr)
    {
        struct ExprVisitor {
            Generator* gen = nullptr;
            PrimitiveType type = PrimitiveType::unknown;

            void operator()(ast::NodeTerm* term)
            {
                type = gen->ast_term(term);
            }
            void operator()(ast::NodeExprBin* expr_bin)
            {
                type = gen->ast_expr_bin(expr_bin);
            }
        };
        ExprVisitor visitor { this };
        std::visit(visitor, expr->var);
        assert(visitor.type != PrimitiveType::unknown);
        return visitor.type;
    }

    void ast_scope(
        const ast::NodeScope* scope, ScopeType type, const std::optional<std::string>& break_label = std::nullopt)
    {
        begin_scope(type, break_label);
        if (scope->block.has_value()) {
            ast_block(scope->block.value());
        }
        end_scope();
    }

    void ast_stmt(const ast::NodeStmt* stmt)
    {
        struct StmtVisitor {
            Generator* gen = nullptr;

            void operator()(ast::NodeStmtPrint* stmt_print) const
            {
                gen->ast_expr(stmt_print->expr);
                gen->pop("rdi");
                gen->print_i64();
                gen->print_newline();
            }
            void operator()(ast::NodeStmtVar* stmt_var) const
            {
                if (!gen->m_vars_lookup.contains(stmt_var->tok_ident->value)) {
                    PrimitiveType type = gen->ast_expr(stmt_var->expr);
                    gen->name_var(stmt_var->tok_ident->value);
                }
                else {
                    std::cerr << "[Error] Identifier already defined: " << stmt_var->tok_ident->value << std::endl;
                    ::exit(EXIT_FAILURE);
                }
            }
            void operator()(ast::NodeStmtEq* stmt_eq) const
            {
                std::optional<StackItem> var = gen->stack_item_by_name(stmt_eq->tok_ident->value);
                if (!var.has_value()) {
                    std::cerr << "[Error] Unknown identifier" << std::endl;
                    ::exit(EXIT_FAILURE);
                }
                PrimitiveType type = gen->ast_expr(stmt_eq->expr);
                if (type != gen->m_stack.at(gen->m_vars_lookup.at(stmt_eq->tok_ident->value)).type) {
                    std::cerr << "[Error] Invalid type" << std::endl;
                    ::exit(EXIT_FAILURE);
                }
                gen->pop("rax");
                gen->m_file << "    mov QWORD [rsp + 8*"
                            << gen->m_stack.at(gen->m_vars_lookup.at(stmt_eq->tok_ident->value))
                                   .stack_offset(gen->m_stack.size())
                            << "], rax\n";
            }
            void operator()(ast::NodeStmtWrite* stmt_write) const
            {
                gen->ast_expr(stmt_write->expr1);
                gen->ast_expr(stmt_write->expr2);
                gen->pop("rdx");
                gen->pop("rsi");
                gen->m_file << "    mov rax, 1\n";
                gen->m_file << "    mov rdi, 1\n";
                gen->m_file << "    syscall\n";
            }
            void operator()(ast::NodeStmtBreak* stmt_break) const
            {
                std::optional<std::string> break_label;
                int scope_count = -1;
                for (const auto& m_scope : std::ranges::reverse_view(gen->m_scopes)) {
                    if (m_scope.type == ScopeType::loop && m_scope.break_label.has_value()) {
                        break_label = m_scope.break_label.value();
                        break;
                    }
                    scope_count++;
                }
                if (!break_label.has_value()) {
                    std::cerr << "[Error] Attempt to break outside of loop" << std::endl;
                    ::exit(EXIT_FAILURE);
                }
                for (int i = 0; i < scope_count; i++) {
                    gen->end_scope();
                }
                gen->m_file << "    jmp " << break_label.value() << "\n";
            }
            void operator()(ast::NodeStmtExpr* stmt_expr) const
            {
                gen->ast_expr(stmt_expr->expr);
            }
        };

        m_file << "    ;; -- stmt --\n";
        StmtVisitor visitor { this };
        std::visit(visitor, stmt->var);
    }

    void ast_control(const ast::NodeControl* control)
    {
        struct ControlVisitor {
            Generator* gen = nullptr;

            void operator()(ast::NodeControlFor* stmt_for) const
            {
                gen->begin_scope(ScopeType::generic);
                gen->ast_stmt(stmt_for->init_stmt);
                const std::string begin_label = gen->get_next_label();
                const std::string end_label = gen->get_next_label();
                gen->m_file << begin_label << ":\n";
                gen->begin_scope(ScopeType::loop, end_label);
                gen->ast_expr(stmt_for->expr);
                gen->pop("rax");
                gen->m_file << "    test rax, rax\n";
                gen->m_file << "    jz " << end_label << "\n";
                gen->ast_scope(stmt_for->scope, ScopeType::loop, end_label);
                gen->ast_stmt(stmt_for->loop_stmt);
                gen->end_scope();
                gen->m_file << "    jmp " << begin_label << "\n";
                gen->m_file << end_label << ":\n";
                gen->end_scope();
            }
            void operator()(ast::NodeControlWhile* stmt_while) const
            {
                const std::string begin_label = gen->get_next_label();
                const std::string end_label = gen->get_next_label();
                gen->m_file << begin_label << ":\n";
                gen->ast_expr(stmt_while->expr);
                gen->pop("rax");
                gen->m_file << "    test rax, rax\n";
                gen->m_file << "    jz " << end_label << "\n";
                gen->ast_scope(stmt_while->scope, ScopeType::loop, end_label);
                gen->m_file << "    jmp " << begin_label << "\n";
                gen->m_file << end_label << ":\n";
            }
            void operator()(ast::NodeControlScope* stmt_scope) const
            {
                gen->ast_scope(stmt_scope->scope, ScopeType::generic);
            }
            void operator()(ast::NodeControlIf* stmt_if) const
            {
                gen->ast_expr(stmt_if->expr);
                const std::string else_label = gen->get_next_label();
                gen->pop("rax");
                gen->m_file << "    test rax, rax\n";
                gen->m_file << "    jz " << else_label << "\n";
                gen->ast_scope(stmt_if->scope, ScopeType::generic);
                if (stmt_if->else_.has_value()) {
                    const std::string end_label = gen->get_next_label();
                    gen->m_file << "    jmp " << end_label << "\n";
                    gen->m_file << else_label << ":\n";
                    gen->ast_scope(stmt_if->else_.value()->scope, ScopeType::generic);
                    gen->m_file << end_label << ":\n";
                }
                else {
                    gen->m_file << else_label << ":\n";
                }
            }
        };
        m_file << "    ;; -- control --\n";
        ControlVisitor visitor { this };
        std::visit(visitor, control->var);
    }

    void ast_block(const ast::NodeBlock* block)
    {
        struct BlockVisitor {
            Generator* gen = nullptr;

            void operator()(const ast::NodeBlockStmt* block_stmt) const
            {
                gen->ast_stmt(block_stmt->stmt);
                if (block_stmt->next.has_value()) {
                    gen->ast_block(block_stmt->next.value());
                }
            }

            void operator()(const ast::NodeBlockControl* block_control) const
            {
                gen->ast_control(block_control->control);
                if (block_control->next.has_value()) {
                    gen->ast_block(block_control->next.value());
                }
            }
        };

        BlockVisitor visitor { this };
        std::visit(visitor, block->var);
    }

    void name_var(const std::string& name)
    {
        assert(!m_vars_lookup.contains(name));
        m_vars_lookup.insert({ name, m_stack.size() - 1 });
        m_stack.back().name = name;
    }

    void push(PrimitiveType type, const std::string& str)
    {
        m_file << "    push " << str << "\n";
        m_stack.push_back({ .stack_index = m_stack.size(), .type = type });
    }

    void pop(const std::optional<std::string>& str = std::nullopt)
    {
        if (str.has_value()) {
            m_file << "    pop " << str.value() << "\n";
        }
        else {
            m_file << "    add rsp, 8\n";
        }
        if (m_stack.back().name.has_value()) {
            m_vars_lookup.erase(m_stack.back().name.value());
        }
        m_stack.pop_back();
    }

    std::string get_next_label()
    {
        return ".L" + std::to_string(m_label_count++);
    }

    void begin_scope(ScopeType type, const std::optional<std::string>& break_label = std::nullopt)
    {
        m_scopes.push_back({ .type = type, .begin_stack_index = m_stack.size(), .break_label = break_label });
    }

    void end_scope()
    {
        int pop_count = 0;
        while (m_stack.size() > m_scopes.back().begin_stack_index) {
            if (m_stack.back().name.has_value()) {
                m_vars_lookup.erase(m_stack.back().name.value());
            }
            m_stack.pop_back();
            pop_count++;
        }
        m_scopes.pop_back();
        if (pop_count != 0) {
            m_file << "    add rsp, " << 8 * pop_count << "\n";
        }
    }

private:
    struct Scope {
        ScopeType type;
        size_t begin_stack_index;
        std::optional<std::string> break_label;
    };

    struct StackItem {
        size_t stack_index;
        PrimitiveType type;
        std::optional<std::string> name;

        [[nodiscard]] size_t stack_offset(size_t stack_size) const
        {
            return stack_size - 1 - stack_index;
        }
    };

    [[nodiscard]] std::optional<std::reference_wrapper<StackItem>> stack_item_by_name(const std::string& name)
    {
        if (!m_vars_lookup.contains(name)) {
            return {};
        }
        return m_stack.at(m_vars_lookup.at(name));
    }

    std::fstream& m_file;
    std::vector<StackItem> m_stack;
    std::unordered_map<std::string, size_t> m_vars_lookup {};
    std::vector<Scope> m_scopes {};
    int m_label_count;
    int m_data_count;
    std::stringstream m_data_stream;
};
