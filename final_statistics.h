#ifndef FINAL_STATISTICS_H
#define FINAL_STATISTICS_H

#include <memory>

namespace QueueingSystem
{
    struct SourceFinalStats
    {
        int requestsCount{};
        double rejectionProbability{};
        double averageBufferTime{};
        double averageServiceTime{};
        double averageProcessingTime{};
        double bufferTimeDispersion{};
        double serviceTimeDispersion{};
    };

    struct DeviceFinalStats
    {
        int requestsCount{};
        double averageServiceTime{};
        double utilizationFactor{};
    };

    using USourceFinalStats = std::unique_ptr<SourceFinalStats>;
    using UDeviceFinalStats = std::unique_ptr<DeviceFinalStats>;

    struct SystemFinalStats
    {
        std::vector<USourceFinalStats> sourcesFinalStats{};
        std::vector<UDeviceFinalStats> deviceFinalStats{};
        double rejectionProbability{};
        double workload{};
        int requiredRequestsCount{};
    };

    using USystemFinalStats = std::unique_ptr<SystemFinalStats>;
}

#endif
