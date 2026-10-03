#include "entity_id.h"
#include <sstream>
#include <iomanip>

namespace origin::core {

std::string EntityId::to_string() const {
    std::ostringstream oss;
    oss << "eid:" << std::hex << std::uppercase << std::setw(16) << std::setfill('0') << id_;
    return oss.str();
}

} // namespace origin::core