#ifndef CLERROR_HPP
#define CLERROR_HPP

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

inline const char* clErrorString(const cl_int err) {
    const char* errStr = "Unknown OpenCL error";

    switch (err) {
        case CL_DEVICE_NOT_FOUND:                         errStr = "CL_DEVICE_NOT_FOUND"; break;
        case CL_DEVICE_NOT_AVAILABLE:                     errStr = "CL_DEVICE_NOT_AVAILABLE"; break;
        case CL_COMPILER_NOT_AVAILABLE:                   errStr = "CL_COMPILER_NOT_AVAILABLE"; break;
        case CL_MEM_OBJECT_ALLOCATION_FAILURE:            errStr = "CL_MEM_OBJECT_ALLOCATION_FAILURE"; break;
        case CL_OUT_OF_RESOURCES:                         errStr = "CL_OUT_OF_RESOURCES"; break;
        case CL_OUT_OF_HOST_MEMORY:                       errStr = "CL_OUT_OF_HOST_MEMORY"; break;
        case CL_PROFILING_INFO_NOT_AVAILABLE:             errStr = "CL_PROFILING_INFO_NOT_AVAILABLE"; break;
        case CL_MEM_COPY_OVERLAP:                         errStr = "CL_MEM_COPY_OVERLAP"; break;
        case CL_IMAGE_FORMAT_MISMATCH:                    errStr = "CL_IMAGE_FORMAT_MISMATCH"; break;
        case CL_IMAGE_FORMAT_NOT_SUPPORTED:               errStr = "CL_IMAGE_FORMAT_NOT_SUPPORTED"; break;
        case CL_BUILD_PROGRAM_FAILURE:                    errStr = "CL_BUILD_PROGRAM_FAILURE"; break;
        case CL_MAP_FAILURE:                              errStr = "CL_MAP_FAILURE"; break;
        case CL_MISALIGNED_SUB_BUFFER_OFFSET:             errStr = "CL_MISALIGNED_SUB_BUFFER_OFFSET"; break;
        case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:errStr = "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST"; break;
        case CL_COMPILE_PROGRAM_FAILURE:                  errStr = "CL_COMPILE_PROGRAM_FAILURE"; break;
        case CL_LINKER_NOT_AVAILABLE:                     errStr = "CL_LINKER_NOT_AVAILABLE"; break;
        case CL_LINK_PROGRAM_FAILURE:                     errStr = "CL_LINK_PROGRAM_FAILURE"; break;
        case CL_DEVICE_PARTITION_FAILED:                  errStr = "CL_DEVICE_PARTITION_FAILED"; break;
        case CL_KERNEL_ARG_INFO_NOT_AVAILABLE:            errStr = "CL_KERNEL_ARG_INFO_NOT_AVAILABLE"; break;
        case CL_INVALID_VALUE:                            errStr = "CL_INVALID_VALUE"; break;
        case CL_INVALID_DEVICE_TYPE:                      errStr = "CL_INVALID_DEVICE_TYPE"; break;
        case CL_INVALID_PLATFORM:                         errStr = "CL_INVALID_PLATFORM"; break;
        case CL_INVALID_DEVICE:                           errStr = "CL_INVALID_DEVICE"; break;
        case CL_INVALID_CONTEXT:                          errStr = "CL_INVALID_CONTEXT"; break;
        case CL_INVALID_QUEUE_PROPERTIES:                 errStr = "CL_INVALID_QUEUE_PROPERTIES"; break;
        case CL_INVALID_COMMAND_QUEUE:                    errStr = "CL_INVALID_COMMAND_QUEUE"; break;
        case CL_INVALID_HOST_PTR:                         errStr = "CL_INVALID_HOST_PTR"; break;
        case CL_INVALID_MEM_OBJECT:                       errStr = "CL_INVALID_MEM_OBJECT"; break;
        case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:          errStr = "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR"; break;
        case CL_INVALID_IMAGE_SIZE:                       errStr = "CL_INVALID_IMAGE_SIZE"; break;
        case CL_INVALID_SAMPLER:                          errStr = "CL_INVALID_SAMPLER"; break;
        case CL_INVALID_BINARY:                           errStr = "CL_INVALID_BINARY"; break;
        case CL_INVALID_BUILD_OPTIONS:                    errStr = "CL_INVALID_BUILD_OPTIONS"; break;
        case CL_INVALID_PROGRAM:                          errStr = "CL_INVALID_PROGRAM"; break;
        case CL_INVALID_PROGRAM_EXECUTABLE:               errStr = "CL_INVALID_PROGRAM_EXECUTABLE"; break;
        case CL_INVALID_KERNEL_NAME:                      errStr = "CL_INVALID_KERNEL_NAME"; break;
        case CL_INVALID_KERNEL_DEFINITION:                errStr = "CL_INVALID_KERNEL_DEFINITION"; break;
        case CL_INVALID_KERNEL:                           errStr = "CL_INVALID_KERNEL"; break;
        case CL_INVALID_ARG_INDEX:                        errStr = "CL_INVALID_ARG_INDEX"; break;
        case CL_INVALID_ARG_VALUE:                        errStr = "CL_INVALID_ARG_VALUE"; break;
        case CL_INVALID_ARG_SIZE:                         errStr = "CL_INVALID_ARG_SIZE"; break;
        case CL_INVALID_KERNEL_ARGS:                      errStr = "CL_INVALID_KERNEL_ARGS"; break;
        case CL_INVALID_WORK_DIMENSION:                   errStr = "CL_INVALID_WORK_DIMENSION"; break;
        case CL_INVALID_WORK_GROUP_SIZE:                  errStr = "CL_INVALID_WORK_GROUP_SIZE"; break;
        case CL_INVALID_WORK_ITEM_SIZE:                   errStr = "CL_INVALID_WORK_ITEM_SIZE"; break;
        case CL_INVALID_GLOBAL_OFFSET:                    errStr = "CL_INVALID_GLOBAL_OFFSET"; break;
        case CL_INVALID_EVENT_WAIT_LIST:                  errStr = "CL_INVALID_EVENT_WAIT_LIST"; break;
        case CL_INVALID_EVENT:                            errStr = "CL_INVALID_EVENT"; break;
        case CL_INVALID_OPERATION:                        errStr = "CL_INVALID_OPERATION"; break;
        case CL_INVALID_GL_OBJECT:                        errStr = "CL_INVALID_GL_OBJECT"; break;
        case CL_INVALID_BUFFER_SIZE:                      errStr = "CL_INVALID_BUFFER_SIZE"; break;
        case CL_INVALID_MIP_LEVEL:                        errStr = "CL_INVALID_MIP_LEVEL"; break;
        case CL_INVALID_GLOBAL_WORK_SIZE:                 errStr = "CL_INVALID_GLOBAL_WORK_SIZE"; break;
        case CL_INVALID_PROPERTY:                         errStr = "CL_INVALID_PROPERTY"; break;
        case CL_INVALID_IMAGE_DESCRIPTOR:                 errStr = "CL_INVALID_IMAGE_DESCRIPTOR"; break;
        case CL_INVALID_COMPILER_OPTIONS:                 errStr = "CL_INVALID_COMPILER_OPTIONS"; break;
        case CL_INVALID_LINKER_OPTIONS:                   errStr = "CL_INVALID_LINKER_OPTIONS"; break;
        case CL_INVALID_DEVICE_PARTITION_COUNT:           errStr = "CL_INVALID_DEVICE_PARTITION_COUNT"; break;
        default:                                          break;
    }

    return errStr;
}

#endif //CLERROR_HPP
