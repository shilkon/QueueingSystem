#ifndef BUFFER_HPP
#define BUFFER_HPP

#include "request.h"

#include <vector>

namespace QueueingSystem
{
    class Buffer
    {
    public:
        explicit Buffer(int bufferSize);

        int getSize() const;
        const URequest* getBufferPtr() const;

        bool placeRequestInBuffer(URequest& request);
        URequest selectRequestFromBuffer();

        URequest getLastRejectedRequest();
        bool isRequestsBufferEmpty() const;

        void reset();
        void reset(int newSize);

    private:
        int bufferSize_;
        int bufferIndex_{};
        std::vector<URequest> requestsBuffer_;
        URequest lastRejectedRequest_;
    };
}

#endif