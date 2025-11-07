#include "debug.h"

PFN_vkSetDebugUtilsObjectNameEXT pfnSetDebugUtilsObjectNameEXT;

namespace debug {

void setDebugUtilsObjectName(PFN_vkSetDebugUtilsObjectNameEXT asd)
{
    pfnSetDebugUtilsObjectNameEXT = asd;
}

void SetDebugObjectName(VkDevice device, VkObjectType objectType, uint64_t objectHandle, const char* name)
{
        if (pfnSetDebugUtilsObjectNameEXT) {
            VkDebugUtilsObjectNameInfoEXT nameInfo = {};
            nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
            nameInfo.objectType = objectType;
            nameInfo.objectHandle = objectHandle;
            nameInfo.pObjectName = name;

            pfnSetDebugUtilsObjectNameEXT(device, &nameInfo);
        }
    }
}
