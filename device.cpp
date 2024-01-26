#include "device.h"

#include <cassert>

namespace QS = QueueingSystem;

QS::Device::Device(int deviceId, double lambda):
    deviceId_(deviceId),
    distribution_(lambda)
{}

int QS::Device::getProcessingRequestSourceId() const
{
    assert(processingRequest_ && "Request is not processed");
    return processingRequest_.get()->id.sourceId;
}

const double* QueueingSystem::Device::getProcessingEndTimePtr() const
{
    return &processingEndTime_;
}

double QS::Device::getProcessingTime() const
{
    return processingTime_;
}

void QS::Device::setLambda(double lambda)
{
    distribution_ = std::exponential_distribution<double>{ lambda };
}

double QS::Device::getLambda() const
{
    return distribution_.lambda();
}

void QS::Device::processRequest(URequest& request, double processingStartTime)
{
    processingRequest_ = std::move(request);
    processingTime_ = MIN_PROCESSING_TIME + distribution_(generator_);
    processingEndTime_ = processingStartTime + processingTime_;
}

void QS::Device::endProcessingRequest()
{
    processingRequest_.reset();
    processingTime_ = IDLE_TIME;
    processingEndTime_ = IDLE_TIME;
}

void QS::Device::reset()
{
    endProcessingRequest();
    distribution_.reset();
}
