#include "precompile.h"

#include <string>
#include <cstdio>
#include "libop/op.h"

#include "parser.h"
#include "exception.h"

static std::string read_full_stream(std::FILE* file) {
    std::string result;
    std::array<char, 4096> buf;
    while (true) {
        auto bytes_read = std::fread(buf.data(), 1, buf.size(), file);
        if (std::ferror(file)) throw kwik::FilesystemError(std::strerror(errno));
        
        if (!bytes_read) break;
        result.insert(result.end(), buf.begin(), buf.begin() + bytes_read);
    }

    return result;
}

static std::string read_file(const std::string& filename) {
    if (filename == "<stdin>") return read_full_stream(stdin);

    auto file = std::fopen(filename.c_str(), "r");
    if (!file) throw kwik::FilesystemError(std::strerror(errno));
    OP_SCOPE_EXIT { std::fclose(file); };
    return read_full_stream(file);
}


int main(int argc, char** argv) {
    std::vector<std::string> args {argv, argv + argc};
    if (args.size() < 2) {
        op::printf("Usage: {} <file>\n", args[0]);
        return 1;
    }

    auto filename = args[1];
    if (filename == "-") filename = "<stdin>";

    try {
        auto src = std::make_shared<std::string>(read_file(args[1]));
        kwik::parse(src, filename);
        return 0;
    } catch (const kwik::CompilationError& e) {
        op::printf("error: {}:{}:{} {}\n", e.filename, e.line, e.col, e.what());
    } catch (const kwik::FilesystemError& e) {
        op::printf("error: {}: {}\n", filename, e.what());
    }

    return 1;
}
