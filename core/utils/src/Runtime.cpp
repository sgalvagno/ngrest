#ifndef WIN32
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#else
#include <windows.h>
#endif
#include <limits.h>
#ifdef __FreeBSD__
#include <sys/types.h>
#include <sys/sysctl.h>
#elif defined __APPLE__
#include <mach-o/dyld.h>
#endif

#include "Exception.h"
#include "File.h"
#include "Runtime.h"

namespace ngrest {

const std::string& Runtime::getApplicationDirPath()
{
    static std::string result;
    if (result.empty()) {
        result = getApplicationFilePath();
        std::string::size_type pos = result.find_last_of(NGREST_PATH_SEPARATOR);
        if (pos != std::string::npos)
            result.erase(pos);
    }

    return result;
}

const std::string& Runtime::getApplicationFilePath()
{
    static std::string result;
    if (!result.empty())
        return result;

#ifdef WIN32
    char path[MAX_PATH];
    DWORD pathSize = GetModuleFileNameA(NULL, path, MAX_PATH);
#else
    char path[PATH_MAX];
#ifdef __FreeBSD__
    size_t pathSize = PATH_MAX;
    int anMib[4];
    anMib[0] = CTL_KERN;
    anMib[1] = KERN_PROC;
    anMib[2] = KERN_PROC_PATHNAME;
    anMib[3] = -1;
    sysctl(anMib, 4, path, &pathSize, NULL, 0);
#else
#ifdef __APPLE__
    uint32_t pathSize = PATH_MAX;
    _NSGetExecutablePath(path, &pathSize);
#else
    int pathSize = readlink("/proc/self/exe", path, PATH_MAX);
#endif
#endif
#endif

    NGREST_ASSERT(pathSize > 0, "Failed to get application file path");

    result.assign(path, pathSize);

    return result;
}

} // namespace ngrest
