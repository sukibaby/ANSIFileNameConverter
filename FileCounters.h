#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <typeinfo>

namespace FileCounters
{
    inline unsigned int changedFileCount = 0;
    inline unsigned int failedFileCount = 0;
    inline unsigned int unchangedFileCount = 0;

    inline void incrementChangedFileCount()
    {
        ++changedFileCount;
    }

    inline void incrementFailedFileCount()
    {
        ++failedFileCount;
    }

    inline void incrementUnchangedFileCount()
    {
        ++unchangedFileCount;
    }

    inline std::string getCounters()
    {
        std::ostringstream oss;
        oss << "Changed: " << changedFileCount << "\n"
            << "Failed: " << failedFileCount << "\n"
            << "Unchanged: " << unchangedFileCount;
        return oss.str();
    }
}
