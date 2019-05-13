#ifndef INET_APPLICATIONS_ICN_ICNLOCALCOMMUNICATOR_H_
#define INET_APPLICATIONS_ICN_ICNLOCALCOMMUNICATOR_H_

#include "inet/common/INETDefs.h"

namespace inet {

class ICNLocalCommunicator: public cSimpleModule {
public:
    /**
     * Constructor.
     */
    ICNLocalCommunicator();

    /**
     * Destructor.
     */
    virtual ~ICNLocalCommunicator();

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage* msg);

private:

    /**
     * Stores whether this communicator should do flooding.
     */
    bool mDoFlooding;

    /**
     * Stores which icn names have been rebroadcasted.
     */
    std::set<std::string> mContentStore;

    /**
     * Helper method to handle messages with kind RECEIVED_PUBLICATION.
     */
    void handleReceivedPublication(cMessage* message);

};

} /* namespace inet */

#endif /* INET_APPLICATIONS_ICN_ICNLOCALCOMMUNICATOR_H_ */
