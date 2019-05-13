
#ifndef INET_APPLICATIONS_ICN_MESSAGEKINDS_H_
#define INET_APPLICATIONS_ICN_MESSAGEKINDS_H_

/**
 * This defines types (kinds) for messages
 * which are exchanged between different ICN
 * modules.
 */
enum MessageKinds {

    // - To ICNBase:
    // --- From ICNPublisher:
    PUBLICATION = 1,
    BROADCAST_PUBLICATION,
    // --- From ICNSubscriber:
    SUBSCRIPTION,
    SILENT_SUBSCRIPTION,
    // - From ICNBase:
    // --- To Subscriber:
    SUBSCRIPTION_RESULT,
    // --- To ICNLocalCommunicator
    REQUEST,
    RECEIVED_PUBLICATION
};



#endif /* INET_APPLICATIONS_ICN_MESSAGEKINDS_H_ */
