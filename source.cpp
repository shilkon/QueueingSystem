#include "source.h"

#include <memory>

namespace QS = QueueingSystem;

QS::Source::Source(int sourceId, double distrRange):
    sourceId_(sourceId),
    distribution_(0.0, distrRange)
{}

const double* QueueingSystem::Source::getNextGenerationTimePtr() const
{
    return &nextGenerationTime_;
}

const int* QueueingSystem::Source::getRequestsCountPtr() const
{
    return &requestsCount_;
}

void QS::Source::setDistributionRange(double distrRange)
{
    distribution_ = std::uniform_real_distribution<double>{ 0.0, distrRange };
}

double QS::Source::getDistributionRange() const
{
    return distribution_.b();
}

std::unique_ptr<QS::Request> QS::Source::generateRequest()
{
    Request newRequest{
        RequestId{
            sourceId_,
            requestsCount_++
        },
        nextGenerationTime_
    };

    nextGenerationTime_ += distribution_(generator_);

    return std::make_unique<Request>(newRequest);
}

void QS::Source::reset()
{
    nextGenerationTime_ = 0.0;
    requestsCount_ = 0;
    distribution_.reset();
}
