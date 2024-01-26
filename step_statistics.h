#ifndef STEP_STATISTICS_H
#define STEP_STATISTICS_H

#include "request.h"

#include <memory>
#include <vector>

namespace QueueingSystem
{
    struct SourceStatus
    {
        const double* nextGenerationTime{};
        const int* requestsCount{};
        const int* rejectionsCount{};
    };

    struct DeviceStatus
    {
        const double* processingEndTime{};
        const int* requestsCount{};
    };

    struct BufferStatus
    {
        const URequest* buffer{};
        int size{};
    };

    struct RequestsCountStatus
    {
        const int* requestCount{};
        int requestsLimit{};
    };

    using USourceStatus = std::unique_ptr<SourceStatus>;
    using UDeviceStatus = std::unique_ptr<DeviceStatus>;
    using UBufferStatus = std::unique_ptr<BufferStatus>;
    using URequestsCountStatus = std::unique_ptr<RequestsCountStatus>;

    struct SystemStatus
    {
        std::vector<USourceStatus> sourcesStatus{};
        std::vector<UDeviceStatus> devicesStatus{};
        UBufferStatus bufferStatus{};
        URequestsCountStatus requestsCountStatus{};
    };
}

#endif
