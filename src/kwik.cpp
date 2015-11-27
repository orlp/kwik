#include "precompile.h"

#include <string>
#include <cstdio>
#include "libop/op.h"

#include "parser.h"
#include "exception.h"
#include "io.h"


using namespace kwik;

int main(int argc, char** argv) {
    std::vector<std::string> args {argv, argv + argc};
    if (args.size() < 2) {
        op::printf("Usage: {} <file>\n", args[0]);
        return 1;
    }

    try {
        Source src = args[1] == "-" ? read_stdin() : read_file(args[1]);
        parse(src);
        return 0;
    } catch (const CompilationError& e) {
        op::print(e.what());
    } catch (const EncodingError& e) {
        op::print(e.what());
    } catch (const FilesystemError& e) {
        op::printf("error: {}: {}\n", args[1], e.what());
    } catch (const InternalCompilerError& e) {
        op::printf("internal compiler error: {}: {}\n", args[1], e.what());
    }

    return 1;
}
