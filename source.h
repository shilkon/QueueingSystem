#ifndef SOURCE_HPP
#define SOURCE_HPP

#include "buffer.h"

#include <random>
#include <memory>

namespace QueueingSystem
{
    inline constexpr double DISTRIBUTION_RANGE{ 5.0 };

    class Source
    {
    public:
        Source(int sourceId, double distrRange = DISTRIBUTION_RANGE);

        const double* getNextGenerationTimePtr() const;
        const int* getRequestsCountPtr() const;

        void setDistributionRange(double distrRange);
        double getDistributionRange() const;

        URequest generateRequest();

        void reset();

    private:
        int sourceId_;
        double nextGenerationTime_{};
        int requestsCount_{};

        std::mt19937 generator_{ std::random_device{}() };
        std::uniform_real_distribution<double> distribution_;
    };

    using USource = std::unique_ptr<Source>;
}

#endif
