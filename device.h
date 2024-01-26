#ifndef DEVICE_HPP
#define DEVICE_HPP

#include "request.h"

#include <random>
#include <memory>

namespace QueueingSystem
{
    inline constexpr double IDLE_TIME{ -1.0 };
    inline constexpr double MIN_PROCESSING_TIME{ 5.0 };

    class Device
    {
    public:
        Device(int deviceId, double lambda = 0.05);

        int getProcessingRequestSourceId() const;
        const double* getProcessingEndTimePtr() const;

        double getProcessingTime() const;

        void setLambda(double lambda);
        double getLambda() const;

        void processRequest(URequest& request, double processingStartTime);
        void endProcessingRequest();

        void reset();

    private:
        int deviceId_;
        URequest processingRequest_{};
        double processingTime_{ IDLE_TIME };
        double processingEndTime_{ IDLE_TIME };

        std::mt19937 generator_{ std::random_device{}() };
        std::exponential_distribution<double> distribution_;
    };

    using UDevice = std::unique_ptr<Device>;
}

#endif
