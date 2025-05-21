#pragma once

#include <string>
#include "chronovyan/common_types.hpp"

namespace chronovyan {

// Notification service interface
class INotificationService {
public:
    virtual ~INotificationService() = default;
    virtual void notifyModeChange(PerformanceMode new_mode, const std::string& reason) = 0;
    virtual void notifyError(const std::string& error_message) = 0;
};

} // namespace chronovyan 