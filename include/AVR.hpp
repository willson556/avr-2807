#pragma once

#include "Transport.hpp"

namespace AVR
{

class DenonAVR
{
public:
    DenonAVR(Transport &transport)
        : transport{transport}
    {
    }

    virtual void connect();

protected:

private:
    Transport& transport;
};

} // namespace AVR