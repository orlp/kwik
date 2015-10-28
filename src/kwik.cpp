#include "precompile.h"

#include "libop/op.h"
#include "parser.h"
#include "exception.h"

static std::string read_file(const std::string& filename) {
    auto file = std::fopen(filename.c_str(), "r");
    if (!file) throw kwik::FilesystemError(std::strerror(errno));
    OP_SCOPE_EXIT { std::fclose(file); };

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


int main(int argc, char** argv) {
    if (argc < 2) {
        op::printf("Usage: {} <file>", argv[0]);
        return 1;
    }

    try {
        std::string src = read_file(argv[1]);
        void* lemon = LemonParseAlloc(malloc);
        OP_SCOPE_EXIT { LemonParseFree(lemon, free); };
        return 0;
    } catch (const kwik::CompilationError& e) {
        op::printf("error: {}:{}:{} {}\n", argv[1], e.line, e.col, e.what());
    } catch (const kwik::FilesystemError& e) {
        op::printf("error: {}: {}\n", argv[1], e.what());
    }
    
    return 1;
}
