#include "queueing_system.h"

#include <memory>
#include <algorithm>
#include <numeric>

namespace QS = QueueingSystem;

QS::QueueingSystem::QueueingSystem(const SystemConfiguration& conf):
    sources_(conf.sourcesCount),
    devices_(conf.devicesCount),
    buffer_(std::make_unique<Buffer>(conf.bufferSize)),
    requestsLimit_(conf.requestsLimit)
{
    for (int i{ 0 }; i < conf.sourcesCount; ++i)
        sources_[i] = std::make_unique<Source>(i);

    for (int i{ 0 }; i < conf.devicesCount; ++i)
        devices_[i] = std::make_unique<Device>(i);

    calendarOfEvents_ = std::make_unique<CalendarOfEvents>(sources_, devices_);
    stats_ = std::make_unique<Statistics>(sources_, devices_);
}

QS::SystemStatus QS::QueueingSystem::getSystemStatus() const
{
    return SystemStatus{
        stats_->getSourcesStatus(calendarOfEvents_->getSourcesEventTime()),
        stats_->getDevicesStatus(calendarOfEvents_->getDevicesEventTime()),
        std::make_unique<BufferStatus>(
            BufferStatus{ buffer_->getBufferPtr(), buffer_->getSize() }),
        std::make_unique<RequestsCountStatus>(
            RequestsCountStatus{ &requestsCount_, requestsLimit_ })
    };
}

QS::SystemFinalStats QueueingSystem::QueueingSystem::getSystemFinalStats() const
{
    double rejectionProbability{ static_cast<double>(stats_->getRejectionsCount()) / requestsCount_ };
    return SystemFinalStats{
        stats_->getSourcesFinalStats(),
        stats_->getDevicesFinalStats(),
        rejectionProbability,
        stats_->getSystemWorkLoad(),
        static_cast<int>((1.643 * 1.643 * (1 - rejectionProbability)) /
        (rejectionProbability * 0.1 * 0.1))
    };
}

int QS::QueueingSystem::getRequestsLimit() const
{
    return requestsLimit_;
}

void QS::QueueingSystem::reset()
{
    for (const auto& source : sources_)
        source->reset();

    for (const auto& device : devices_)
        device->reset();

    buffer_->reset();

    requestsCount_ = 0;
    deviceIndex_ = 0;

    stats_->reset();
}

void QS::QueueingSystem::reset(const SystemConfiguration& conf)
{
    auto oldSourcesCount{ sources_.size() };
    sources_.resize(conf.sourcesCount);
    if (conf.sourcesCount > oldSourcesCount)
        for (auto i{ oldSourcesCount }; i < conf.sourcesCount; ++i)
            sources_[i] = std::make_unique<Source>(i);

    if (sources_.front()->getDistributionRange() != conf.distrRange)
        for (const auto& source : sources_)
            source->setDistributionRange(conf.distrRange);

    buffer_->reset(conf.bufferSize);

    auto oldDevicesCount{ devices_.size() };
    devices_.resize(conf.devicesCount);
    if (conf.devicesCount > oldDevicesCount)
        for (auto i{ oldDevicesCount }; i < conf.devicesCount; ++i)
            devices_[i] = std::make_unique<Device>(i);

    if (devices_.front()->getLambda() != conf.lambda)
        for (const auto& device : devices_)
            device->setLambda(conf.lambda);

    if (requestsLimit_ != conf.requestsLimit)
        requestsLimit_ = conf.requestsLimit;

    if (oldSourcesCount != conf.sourcesCount || oldDevicesCount != conf.devicesCount)
    {
        calendarOfEvents_ = std::make_unique<CalendarOfEvents>(sources_, devices_);
        stats_ = std::make_unique<Statistics>(sources_, devices_);
    }

    reset();
}

bool QS::QueueingSystem::makeStep()
{
    auto nextDeviceEvent{ calendarOfEvents_->getNextEvent(EventType::deviceEvent) };
    if (requestsCount_ >= requestsLimit_)
    {
        if (**nextDeviceEvent < 0.0)
            return false;
        else
        {
            stats_->setTotalTime(**nextDeviceEvent);
            processEvent(nextDeviceEvent, EventType::deviceEvent);
        }
    }
    else
    {
        auto nextSourceEvent{ calendarOfEvents_->getNextEvent(EventType::sourceEvent) };
        if (**nextSourceEvent < **nextDeviceEvent || **nextDeviceEvent < 0.0)
            processEvent(nextSourceEvent, EventType::sourceEvent);
        else
            processEvent(nextDeviceEvent, EventType::deviceEvent);
    }
    return true;
}

void QS::QueueingSystem::processEvent(const EventConstIter& eventIter, EventType eventType)
{
    double time{ **eventIter };
    auto eventIndex{ calendarOfEvents_->getEventIndex(eventIter, eventType) };

    if (eventType == EventType::sourceEvent)
    {
        auto request{ sources_[eventIndex]->generateRequest() };

        ++requestsCount_;

        if (!buffer_->placeRequestInBuffer(request))
        {
            auto rejectedRequest{ buffer_->getLastRejectedRequest() };
            stats_->incSourceRejectionsCount(rejectedRequest->id.sourceId);
        }

        tryProcessRequest(time);
    }
    else //EventType::deviceEvent
    {
        const auto& device{ devices_[eventIndex] };

        stats_->addDeviceStats(eventIndex, device->getProcessingTime());
        stats_->addSourceServiceTime(device->getProcessingRequestSourceId(),
            device->getProcessingTime());

        device->endProcessingRequest();

        if (!buffer_->isRequestsBufferEmpty())
            tryProcessRequest(time);
    }
}

void QS::QueueingSystem::tryProcessRequest(double startTime)
{
    if (int freeDeviceIndex{ calendarOfEvents_->getFreeDeviceIndex(deviceIndex_) };
        freeDeviceIndex != devices_.size())
    {
        auto request{ buffer_->selectRequestFromBuffer() };

        if (startTime != request->generationTime)
            stats_->addSourceBufferTime(request->id.sourceId,
                startTime - request->generationTime);

        devices_[freeDeviceIndex]->processRequest(request, startTime);

        deviceIndex_ = freeDeviceIndex < devices_.size() - 1 ?
            freeDeviceIndex + 1 : deviceIndex_ = 0;
    }
}
