#pragma once

#include <queue>
#include <vector>

#include "gn10_can/drivers/driver_interface.hpp"

class MockDriver : public gn10_can::drivers::ICanDriver
{
public:
    bool send(const gn10_can::CANFrame& frame) override
    {
        sent_frames.push_back(frame);
        return true;
    }

    bool receive(gn10_can::CANFrame& out_frame) override
    {
        if (receive_queue.empty()) {
            return false;
        }
        out_frame = receive_queue.front();
        receive_queue.pop();
        return true;
    }

    // Helper methods for testing
    void push_receive_frame(const gn10_can::CANFrame& frame)
    {
        receive_queue.push(frame);
    }

    std::vector<gn10_can::CANFrame> sent_frames;
    std::queue<gn10_can::CANFrame> receive_queue;
};
