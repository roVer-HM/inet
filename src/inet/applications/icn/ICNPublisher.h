#ifndef INET_APPLICATIONS_ICN_ICNPUBLISHER_H_
#define INET_APPLICATIONS_ICN_ICNPUBLISHER_H_

#include "inet/common/INETDefs.h"

namespace inet {

class ICNPublisher: public cSimpleModule {
public:
    /**
     * Constructor.
     */
    ICNPublisher();

    /**
     * Destructor.
     */
    virtual ~ICNPublisher();

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage* msg);

private:

    /**
     * Stores the name of the data that is being published.
     */
    std::string mDataName;

    /**
     * Stores the delay between two messages and the time
     * until the first message is sent.
     */
    int mDelay;

    /**
     * Stores whether to repeat publications.
     */
    bool mRepeat;

    /**
     * Stores the size of the data.
     */
    int mDataSize;

    /**
     * Stores the message that will be sent to myself.
     */
    cMessage* mSendMessage;
};

} /* namespace inet */

#endif /* INET_APPLICATIONS_ICN_ICNPUBLISHER_H_ */
