#include "source_location.h"
#include "source_file.h"
#include <sstream>

namespace chronovyan {

std::string SourceLocation::toString() const {
    if (!isValid()) {
        return "<unknown location>";
    }
    
    std::ostringstream oss;
    oss << sourceFile->getName() << ":" << line << ":" << column;
    return oss.str();
}

} // namespace chronovyan 