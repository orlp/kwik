#ifndef KWIK_EXCEPTION_H
#define KWIK_EXCEPTION_H

#include "libop/op.h"

#include "io.h"

namespace kwik {
    struct CompilationError : public virtual op::BaseException {
        SourceRef ref;
        CompilationError(SourceRef ref) : ref(ref), formatted_what() { }

        virtual const char* error_type() const noexcept { return "compilation error"; }
        virtual CompilationError* clone() const { return new CompilationError(*this); }

        const char* what() const noexcept override {
            try {
                if (!formatted_what) formatted_what = std::make_shared<std::string>(format_what());
                return formatted_what->c_str();
            } catch (...) {
                return "another exception was thrown while formatting what() - giving up";
            }
        }

    protected:
        CompilationError();

        virtual std::string format_what() const {
            return op::format("{}:{}:{}: {}: {}\n    {}\n{}^",
                              ref.src.name, ref.line, ref.col, this->error_type(),
                              op::BaseException::what(), 
                              ref.src.lines[ref.line - 1], std::string(ref.col - 1 + 4, ' '));
        }

    private:
        mutable std::shared_ptr<std::string> formatted_what;
    };

    struct SyntaxError : public virtual CompilationError {
        SyntaxError(std::string msg, SourceRef ref)
            : op::BaseException(std::move(msg)), CompilationError(ref) { }
        const char* error_type() const noexcept override { return "syntax error"; }
        CompilationError* clone() const override { return new SyntaxError(*this); }
    protected: SyntaxError() { }
    };

    struct SemanticError : public virtual CompilationError {
        SemanticError(std::string msg, SourceRef ref)
            : op::BaseException(std::move(msg)), CompilationError(ref) { }
        const char* error_type() const noexcept override { return "error"; }
        CompilationError* clone() const override { return new SemanticError(*this); }
    protected: SemanticError() { }
    };

    struct EncodingError : public virtual op::BaseException {
        EncodingError(std::string msg, std::string name, size_t line, size_t col, std::string context_line)
        : op::BaseException(std::move(msg)) {
            formatted_msg = std::make_shared<std::string>(
                op::format("{}:{}:{}: encoding error: {}\n    {}\n{}^",
                    name, line, col, op::BaseException::what(), 
                    context_line, std::string(col - 1 + 4, ' ')));
        }
        const char* what() const noexcept override { return formatted_msg->c_str(); }
    protected:
        EncodingError() { }
    private:
        std::shared_ptr<std::string> formatted_msg;
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
