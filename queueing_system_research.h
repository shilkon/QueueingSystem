#ifndef QUEUEING_SYSTEM_RESEARCH_H
#define QUEUEING_SYSTEM_RESEARCH_H

#include "queueing_system.h"

#include <set>
#include <functional>
#include <vector>

namespace QueueingSystem
{
    inline constexpr double REJECT_PROB_CONSTRAINT{ 0.05 };
    inline constexpr double WORKLOAD_CONSTRAINT{ 0.95 };

    struct SystemConfigurationStats
    {
        USystemConfiguration conf{};
        double rejectionProbability{};
        double workload{};
    };

    inline bool confSatisfyConstraints(const SystemFinalStats& stats);

    using USysConfStats = std::unique_ptr<SystemConfigurationStats>;

    inline bool USysConfStatsCmp(const USysConfStats& left, const USysConfStats& right);

    using ResearchedConfStats = std::set<USysConfStats, decltype(&USysConfStatsCmp)>;
    using GraphicsData = std::pair<std::vector<float>, std::pair<std::vector<float>, std::vector<float>>>;

    ResearchedConfStats researchQueueingSystem(int sourcesCount, float distrRange);

    GraphicsData getGraphicsDataVaryDevicesCount(int bufferSize, float lambda);
    GraphicsData getGraphicsDataVaryLambda(int bufferSize, int devicesCount);
    GraphicsData getGraphicsDataVaryBufferSize(int devicesCount, float lambda);
}

#endif
