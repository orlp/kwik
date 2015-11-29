#ifndef KWIK_IO_H
#define KWIK_IO_H

#include <string>
#include <memory>
#include <vector>

namespace kwik {
    struct Source {
        // To make the lexer fast (lookahead without bounds checks) we append
        // null bytes to code. Subtract this from code.end() when you want a
        // full range containing only the code.
        constexpr static int NULL_BYTES_APPENDED = 8;

        std::string name;
        std::string code;
        std::vector<std::string> lines;
    };

    struct SourceRef {
        const Source& src;
        size_t line;
        size_t col;
    };
    
    Source read_stdin();
    Source read_file(const std::string& filename);
    Source make_source(const std::string& src, const std::string& name);
}



#endif
