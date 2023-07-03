#include <filesystem>
#include <fstream>
#include <iostream>

std::string exec(const std::string& command)
{
    std::array<char, 128> buffer {};
    std::string result;

    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        return "[Error] popen failed";
    }
    while (!feof(pipe)) {
        if (fgets(buffer.data(), 128, pipe) != nullptr) {
            result.append(buffer.data());
        }
    }
    pclose(pipe);
    return result;
}

void run_test(const std::filesystem::path& compiler, const std::filesystem::path& test_dir)
{
    std::filesystem::create_directory(test_dir / "bin");
    system((compiler.string() + " " + (test_dir / (test_dir.filename().string() + ".hy")).string() + " "
            + (test_dir / "bin").string())
               .c_str());
    std::string output = exec((test_dir / "bin" / test_dir.filename().string()).string());

    std::ifstream expected_file((test_dir / "expected.txt").string());
    std::stringstream buffer;
    buffer << expected_file.rdbuf();
    std::string expected = buffer.str();
    expected_file.close();

    if (output != expected) {
        std::cerr << "\033[1;31m[Failed] " << test_dir.filename() << "\033[0m" << std::endl;
        std::cout << "[Expected]" << std::endl;
        std::cout << expected << std::endl;
        std::cout << "[Output]" << std::endl;
        std::cout << output << std::endl;
    }
    else {
        std::cout << "\033[1;32m[Passed] " << test_dir.filename() << "\033[0m" << std::endl;
    }
}

int main(int argc, const char* argv[])
{
    if (argc != 2) {
        std::cerr << "[Error] Expected 1 argument" << std::endl;
        std::cerr << "[Error] run_tests <compiler>" << std::endl;
        exit(EXIT_FAILURE);
    }

    const std::string compiler(argv[1]);

    run_test(compiler, "tests/hello_world");
    run_test(compiler, "tests/simple_math");
    run_test(compiler, "tests/incdec");
    run_test(compiler, "tests/bool");
    run_test(compiler, "tests/string");
    run_test(compiler, "tests/loops");
}