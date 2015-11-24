#ifndef KWIK_EXCEPTION_H
#define KWIK_EXCEPTION_H

#include "libop/op.h"

namespace kwik {
    struct CompilationError : public virtual op::BaseException {
        std::shared_ptr<std::string> name;
        size_t line;
        size_t col;
        std::shared_ptr<std::string> context_line;

        virtual const char* error_type() const noexcept { return "compilation error"; }
        const char* what() const noexcept override {
            try {
                if (!formatted_what) formatted_what = format_what();
                return formatted_what->c_str();
            } catch (...) {
                return "another exception was thrown while formatting what() - giving up";
            }
        }

        virtual ~CompilationError() = default;

    protected:
        CompilationError();
        CompilationError(std::string name, size_t line, size_t col, std::string context_line)
            : name(std::make_shared<std::string>(std::move(name)))
            , line(line), col(col)
            , context_line(std::make_shared<std::string>(std::move(context_line)))
            , formatted_what() { }

        virtual std::string format_what() const {
            return op::format("{}:{}:{}: {}: {}\n    {}\n{}^",
                              *name, line, col, this->error_type(), op::BaseException::what(), 
                              *context_line, std::string(col - 1 + 4, ' '));
        }

    private:
        mutable op::optional<std::string> formatted_what;
    };

    struct SyntaxError : public virtual CompilationError {
        SyntaxError(std::string msg, std::string name, size_t line, size_t col, std::string context_line)
        : op::BaseException(std::move(msg))
        , CompilationError(std::move(name), line, col, std::move(context_line)) { }
        const char* error_type() const noexcept override { return "syntax error"; }
    protected: SyntaxError() { }
    };

    struct EncodingError : public virtual CompilationError {
        EncodingError(std::string msg, std::string name, size_t line, size_t col, std::string context_line)
        : op::BaseException(std::move(msg))
        , CompilationError(std::move(name), line, col, std::move(context_line)) { }
        const char* error_type() const noexcept override { return "encoding error"; }
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
}


#endif
