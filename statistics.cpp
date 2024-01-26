#include "statistics.h"

#include <numeric>

namespace QS = QueueingSystem;

QS::Statistics::Statistics(const std::vector<USource>& sources,
    const std::vector<UDevice>& devices):
    sourcesStats_(sources.size()),
    devicesStats_(devices.size())
{
    for (int i{}; i < sources.size(); ++i)
        sourcesStats_[i] = std::make_unique<SourceStats>(
            SourceStats{ sources[i]->getRequestsCountPtr() }
        );

    for (int i{}; i < devices.size(); ++i)
        devicesStats_[i] = std::make_unique<DeviceStats>();
}

void QS::Statistics::setTotalTime(double time)
{
    if (simTime_ < 0.0)
        simTime_ = time;
    implTime_ = time;
}

double QS::Statistics::getTotalTime() const
{
    return implTime_;
}

void QS::Statistics::reset()
{
    for (const auto& sourceStats : sourcesStats_)
    {
        sourceStats->rejectionsCount = 0;
        sourceStats->bufferTime.clear();
        sourceStats->serviceTime.clear();
    }

    for (const auto& deviceStats : devicesStats_)
    {
        deviceStats->requestsCount = 0;
        deviceStats->serviceTime = 0.0;
    }

    implTime_ = 0.0;
}

void QS::Statistics::incSourceRejectionsCount(int sourceId) const
{
    sourcesStats_[sourceId]->rejectionsCount++;
}

void QS::Statistics::addSourceBufferTime(int sourceId, double time) const
{
    sourcesStats_[sourceId]->bufferTime.push_back(time);
}

void QS::Statistics::addSourceServiceTime(int sourceId, double time) const
{
    sourcesStats_[sourceId]->serviceTime.push_back(time);
}

void QS::Statistics::addDeviceStats(int deviceId, double time) const
{
    devicesStats_[deviceId]->requestsCount++;
    devicesStats_[deviceId]->serviceTime += time;
}

std::vector<QS::USourceStatus> QS::Statistics::getSourcesStatus(std::vector<const double*> sourcesEventTime) const
{
    std::vector<USourceStatus> sourcesStatus{ sourcesStats_.size() };
    for (int i{}; i < sourcesStats_.size(); ++i)
        sourcesStatus[i] = std::make_unique<SourceStatus>(
            SourceStatus{
                sourcesEventTime[i],
                sourcesStats_[i]->requestsCount,
                &(sourcesStats_[i]->rejectionsCount)
            });

    return sourcesStatus;
}

std::vector<QS::UDeviceStatus> QS::Statistics::getDevicesStatus(std::vector<const double*> devicesEventTime) const
{
    std::vector<UDeviceStatus> devicesStatus{ devicesStats_.size() };
    for (int i{}; i < devicesStats_.size(); ++i)
        devicesStatus[i] = std::make_unique<DeviceStatus>(
            DeviceStatus{
                devicesEventTime[i],
                &(devicesStats_[i]->requestsCount),
            });

    return devicesStatus;
}

std::vector<QS::USourceFinalStats> QS::Statistics::getSourcesFinalStats() const
{
    std::vector<USourceFinalStats> sourcesFinalStats{ sourcesStats_.size() };
    for (int sourceId{}; sourceId < sourcesStats_.size(); sourceId++)
        sourcesFinalStats[sourceId] = getSourceFinalStats(sourceId);

    return sourcesFinalStats;
}

std::vector<QS::UDeviceFinalStats> QS::Statistics::getDevicesFinalStats() const
{
    std::vector<UDeviceFinalStats> devicesFinalStats{ devicesStats_.size() };
    for (int deviceId{}; deviceId < devicesStats_.size(); ++deviceId)
        devicesFinalStats[deviceId] = getDeviceFinalStats(deviceId);

    return devicesFinalStats;
}

double QS::Statistics::getSystemWorkLoad() const
{
    return std::accumulate(devicesStats_.cbegin(), devicesStats_.cend(), 0.0,
        [this](double sum, const auto& next)
        {
            return sum + next->serviceTime / implTime_;
        }) / devicesStats_.size();
}

double QS::Statistics::getSimTime() const
{
    return simTime_;
}

double QS::Statistics::getImplTime() const
{
    return implTime_;
}

QS::USourceFinalStats QS::Statistics::getSourceFinalStats(int sourceId) const
{
    auto sourceFinalStats{ std::make_unique<SourceFinalStats>() };
    const auto& sourceStats{ *sourcesStats_[sourceId] };

    sourceFinalStats->requestsCount = *sourceStats.requestsCount;
    sourceFinalStats->rejectionProbability = static_cast<double>(sourceStats.rejectionsCount) /
        sourceFinalStats->requestsCount;

    sourceFinalStats->averageBufferTime = std::accumulate(sourceStats.bufferTime.cbegin(),
        sourceStats.bufferTime.cend(), 0.0) / (*sourceStats.requestsCount - sourceStats.rejectionsCount);
    sourceFinalStats->averageServiceTime = std::accumulate(sourceStats.serviceTime.cbegin(),
        sourceStats.serviceTime.cend(), 0.0) / (*sourceStats.requestsCount - sourceStats.rejectionsCount);
    sourceFinalStats->averageProcessingTime = sourceFinalStats->averageBufferTime +
        sourceFinalStats->averageServiceTime;

    sourceFinalStats->bufferTimeDispersion = getDispersion(sourceStats.bufferTime,
        sourceFinalStats->averageBufferTime);
    sourceFinalStats->serviceTimeDispersion = getDispersion(sourceStats.serviceTime,
        sourceFinalStats->averageServiceTime);

    return std::move(sourceFinalStats);
}

QS::UDeviceFinalStats QS::Statistics::getDeviceFinalStats(int deviceId) const
{
    auto deviceFinalStats{ std::make_unique<DeviceFinalStats>() };
    const auto& deviceStats{ *devicesStats_[deviceId] };

    deviceFinalStats->requestsCount = deviceStats.requestsCount;
    deviceFinalStats->averageServiceTime = deviceStats.serviceTime / deviceStats.requestsCount;
    deviceFinalStats->utilizationFactor = deviceStats.serviceTime / implTime_;

    return std::move(deviceFinalStats);
}

int QS::Statistics::getRejectionsCount() const
{
    return std::accumulate(sourcesStats_.cbegin(), sourcesStats_.cend(), 0,
        [](int sum, const auto& sourceStats)
        {
            return sum + sourceStats->rejectionsCount;
        });
}

double QS::Statistics::getDispersion(const std::vector<double>& timeVector, double averageTime) const
{
    return averageTime == 0 ? 0.0 : std::accumulate(timeVector.cbegin(), timeVector.cend(), 0.0,
        [&](double sum, double time)
        {
            return sum + (time - averageTime) * (time - averageTime);
        }) / timeVector.size();
}
