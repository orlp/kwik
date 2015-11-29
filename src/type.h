#ifndef KWIK_TYPE_H
#define KWIK_TYPE_H

#include "exception.h"

namespace kwik {
    enum class Type {
        UNKNOWN = 0,
        I64,
    };

    inline std::string type_name(Type type) {
        switch (type) {
        case Type::UNKNOWN: return "Unknown";
        case Type::I64: return "I64";
        }

        throw InternalCompilerError("type_name unexpected type");
    }
}

#endif
