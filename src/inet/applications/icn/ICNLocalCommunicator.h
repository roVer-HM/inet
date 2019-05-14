#ifndef INET_APPLICATIONS_ICN_ICNLOCALCOMMUNICATOR_H_
#define INET_APPLICATIONS_ICN_ICNLOCALCOMMUNICATOR_H_

#include "inet/common/INETDefs.h"
#include "inet/applications/icn/ICNName.h"

namespace inet {

class ICNLocalCommunicator: public cSimpleModule, public cListener {
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

    /**
     * Override receive signal from cListener.
     *
     * We listen to l2AssociatedSignal and l2BeaconLostSignal.
     */
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *details) override;

private:

    /**
     * Stores whether this communicator should do flooding.
     */
    bool mDoFlooding;

    /**
     * Stores the delay between two requests.
     */
    int mRequestDelay;

    /**
     * Stores the size of requests.
     */
    int mRequestSize;

    /**
     * Stores the name of the data
     * to be requested.
     */
    ICNName mRequestedDataName;

    /**
     * Stores the size of a publication.
     */
    int mPublicationSize;

    /**
     * Stores wheter this is associated to an ap right now.
     */
    bool mAssociatedToAP;

    /**
     * The message which will be sent to myself to initiate sending of a request
     * to icn base.
     */
    cMessage* mRequestMessage;

    /**
     * Stores which icn names have been rebroadcasted.
     */
    std::vector<ICNName> mContentStore;

    /**
     * Helper method to handle messages with kind RECEIVED_PUBLICATION.
     */
    void handleReceivedPublication(cMessage* message);

    /**
     * Handle self message: mRequestMessage.
     */
    void handleSelfMessageRequest(void);

    /**
     * Checks if we can answer the received request and does so if possible.
     */
    void handleReceivedRequest(cMessage* message);

    /**
     * Attempts to find matching data in the content store. If it
     * finds something it will return true and the index parameter will
     * carry the index at that the data was found. Prefix mathced names
     * are ignored.
     */
    bool findInContentStore(ICNName& name, size_t& index);

    /**
     * Adds the given icn name to the content store. Prefix matched
     * names are ignored.
     *
     * @return true if something was added to the content store
     */
    bool addToContentStore(ICNName& name);
};

} /* namespace inet */

#endif /* INET_APPLICATIONS_ICN_ICNLOCALCOMMUNICATOR_H_ */
