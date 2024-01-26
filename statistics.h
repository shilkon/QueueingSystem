#ifndef STATISTICS_H
#define STATISTICS_H

#include "step_statistics.h"
#include "final_statistics.h"
#include "source.h"
#include "device.h"

#include <vector>
#include <memory>

namespace QueueingSystem
{
    class Statistics
    {
    public:
        Statistics(const std::vector<USource>& sources,
            const std::vector<UDevice>& devices);

        void setTotalTime(double time);
        double getTotalTime() const;

        void reset();

        void incSourceRejectionsCount(int sourceId) const;
        void addSourceBufferTime(int sourceId, double time) const;
        void addSourceServiceTime(int sourceId, double time) const;
        void addDeviceStats(int deviceId, double time) const;

        std::vector<USourceStatus> getSourcesStatus(std::vector<const double*> sourcesEventTime) const;
        std::vector<UDeviceStatus> getDevicesStatus(std::vector<const double*> devicesEventTime) const;

        std::vector<USourceFinalStats> getSourcesFinalStats() const;
        std::vector<UDeviceFinalStats> getDevicesFinalStats() const;
        double getSystemWorkLoad() const;
        double getSimTime() const;
        double getImplTime() const;

        int getRejectionsCount() const;

    private:
        struct SourceStats
        {
            const int* requestsCount{};
            int rejectionsCount{};
            std::vector<double> bufferTime{};
            std::vector<double> serviceTime{};
        };

        struct DeviceStats
        {
            int requestsCount{};
            double serviceTime{};
        };

        USourceFinalStats getSourceFinalStats(int sourceId) const;
        UDeviceFinalStats getDeviceFinalStats(int deviceId) const;
        double getDispersion(const std::vector<double>& timeVector, double averageTime) const;

        std::vector<std::unique_ptr<SourceStats>> sourcesStats_;
        std::vector<std::unique_ptr<DeviceStats>> devicesStats_;
        double simTime_{ -1.0 };
        double implTime_{ -1.0 };
    };
}

#endif
