#include "buffer.h"

#include <algorithm>

namespace QS = QueueingSystem;

QS::Buffer::Buffer(int bufferSize):
    bufferSize_(bufferSize),
    requestsBuffer_(bufferSize)
{}

int QS::Buffer::getSize() const
{
    return requestsBuffer_.size();
}

const QS::URequest* QueueingSystem::Buffer::getBufferPtr() const
{
    return requestsBuffer_.data();
}


bool QS::Buffer::placeRequestInBuffer(URequest& request)
{
    if (bufferIndex_ != bufferSize_)
    {
        requestsBuffer_[bufferIndex_] = std::move(request);
        ++bufferIndex_;
        return true;
    }
    else
    {
        lastRejectedRequest_ = std::move(requestsBuffer_.back());
        requestsBuffer_.back() = std::move(request);
        return false;
    }
}

std::unique_ptr<QS::Request> QS::Buffer::selectRequestFromBuffer()
{
    if (!bufferIndex_)
    {
        return URequest{};
    }

    auto bufferStart = requestsBuffer_.begin();
    auto requestIter = std::min_element(bufferStart, bufferStart + bufferIndex_,
        [](const auto& first, const auto& second)
        {
            return *first < *second;
        });
    auto request{ std::move(requestsBuffer_[requestIter - bufferStart]) };

    std::rotate(requestIter, requestIter + 1, bufferStart + bufferIndex_);
    --bufferIndex_;

    return std::move(request);
}

QS::URequest QueueingSystem::Buffer::getLastRejectedRequest()
{
    return std::move(lastRejectedRequest_);
}

bool QS::Buffer::isRequestsBufferEmpty() const
{
    return !bufferIndex_;
}

void QS::Buffer::reset()
{
    bufferIndex_ = 0;
    for (auto& position : requestsBuffer_)
        position.reset();
    lastRejectedRequest_.reset();
}

void QS::Buffer::reset(int newSize)
{
    bufferSize_ = newSize;
    requestsBuffer_.resize(newSize);
    reset();
}
