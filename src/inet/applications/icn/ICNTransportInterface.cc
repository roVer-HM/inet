
#include "inet/applications/icn/ICNTransportInterface.h"

namespace inet {


ICNTransportInterface::ICNTransportInterface(void)
: mCallback(nullptr)
{
}

ICNTransportInterface::~ICNTransportInterface(void) {
    // don't do anything because we are not the owner of the callback
}

void ICNTransportInterface::setCallback(ICNTransportInterfaceCallback* callback) {
    mCallback = callback;
}

ICNTransportInterfaceCallback* ICNTransportInterface::getCallback(void) {
    return mCallback;
}


} // end namespace inet




