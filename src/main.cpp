#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <optional>
#include <vector>

#include "ast.hpp"
#include "gen.hpp"
#include "parse.hpp"
#include "token.hpp"

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

    std::cout << "[Info] Compiling: " << input_file_path.string() << std::endl;

    std::cout << "[Progress] Tokenizing" << std::endl;

    std::vector<Token> file_tokens = tokenize_file(input_file_path);
    //    for (const Token& token : file_tokens) {
    //        std::cout << token.value << "\t" << to_string(token.type) << std::endl;
    //    }

    std::cout << "[Progress] Parsing" << std::endl;

    Parser parser(std::move(file_tokens));

    ast::NodeExpr expr_node = parser.parse_expr();

    std::cout << "[Progress] Generating" << std::endl;
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
    }

    std::cout << "[Progress] Assembling" << std::endl;
    std::stringstream nasm_cmd;
    nasm_cmd << "nasm -felf64 " << input_dir / input_filename << ".asm";
    int return_val = system(nasm_cmd.str().c_str());

    if (return_val != 0) {
        std::cerr << "[Error] Assembling failed\n";
        exit(EXIT_FAILURE);
    }

    std::cout << "[Progress] Linking" << std::endl;
    std::stringstream ld_cmd;
    ld_cmd << "ld " << input_dir / input_filename << ".o -o " << input_filename;
    return_val = system(ld_cmd.str().c_str());

    if (return_val != 0) {
        std::cerr << "[Error] Linking failed\n";
        exit(EXIT_FAILURE);
    }

    std::cout << "[Progress] Done" << std::endl;
    return EXIT_SUCCESS;
}