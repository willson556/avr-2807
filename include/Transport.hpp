#pragma once

#include <vector>
#include <cstdlib>

namespace AVR
{

class Transport
{
public:
    virtual void connect() const = 0;
    virtual void write(const std::vector<uint8_t> &data) const = 0;
    virtual std::vector<uint8_t> read(size_t length, bool wait_for_all) const = 0;
    virtual void flush() const = 0;
};

}