#ifndef KWIK_EXCEPTION_H
#define KWIK_EXCEPTION_H

#include "libop/op.h"

namespace kwik {
    struct CompilationError : public virtual op::BaseException {
        std::string name;
        size_t line;
        size_t col;

    protected:
        CompilationError();
        CompilationError(std::string name, size_t line, size_t col)
        : name(std::move(name)), line(line), col(col) { }
    };

    struct SyntaxError : public virtual CompilationError {
        SyntaxError(std::string msg, std::string name, size_t line, size_t col)
        : op::BaseException(std::move(msg)), CompilationError(name, line, col) { }
    protected: SyntaxError() { }
    };

    struct EncodingError : public virtual CompilationError {
        EncodingError(std::string msg, std::string name, size_t line, size_t col)
        : op::BaseException(std::move(msg)), CompilationError(name, line, col) { }
    protected: EncodingError() { }
    };

    struct FilesystemError : public virtual op::BaseException {
        FilesystemError(std::string msg) : op::BaseException(std::move(msg)) { }
    protected: FilesystemError() { }
    };

    struct InternalCompilerError : public virtual op::BaseException {
        InternalCompilerError(std::string msg) : op::BaseException(std::move(msg)) { }
    protected: InternalCompilerError() { }
    };

    struct StopCompilation { };
}


#endif
