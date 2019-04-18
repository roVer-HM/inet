//
// Copyright (C) 2000 Institut fuer Telematik, Universitaet Karlsruhe
// Copyright (C) 2004,2011 Andras Varga
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#ifndef __INET_UDPBASICAPP_H
#define __INET_UDPBASICAPP_H

#include <vector>

#include "inet/common/INETDefs.h"

#include "inet/applications/base/ApplicationBase.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"

namespace inet {

class INET_API ICNUdpInterface : public ApplicationBase, public UdpSocket::ICallback
{
  protected:
    enum SelfMsgKinds { START = 1, SEND, STOP };

    int port;
    std::string mDestinationContentBasedAddress;
    std::string mLocalContentBasedAddress;

    // state
    UdpSocket socket;
    cMessage *selfMsg = nullptr;
    IInterfaceTable* interfaceTableModule = nullptr;


  protected:
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void handleMessageWhenUp(cMessage *msg) override;
    virtual void finish() override;
    virtual void refreshDisplay() const override;

    virtual void sendPacket();
    virtual void processPacket(Packet *msg);

    /**
     * Find the source interface for the given packet. In case
     * there is no InterfaceInd tag or the specified interface
     * does not exist this will return a nullptr.
     *
     * @param packet The packet to examine.
     *
     * @return InterfaceEntry* (nullptr if not found)
     */
    const InterfaceEntry* getSourceInterface(Packet* packet);

    virtual void processStart();
    virtual void processSend();

    virtual void handleStartOperation(LifecycleOperation *operation) override;
    virtual void handleStopOperation(LifecycleOperation *operation) override;
    virtual void handleCrashOperation(LifecycleOperation *operation) override;

    virtual void socketDataArrived(UdpSocket *socket, Packet *packet) override;
    virtual void socketErrorArrived(UdpSocket *socket, Indication *indication) override;
    virtual void socketClosed(UdpSocket *socket) override;

  public:
    ICNUdpInterface() {}
    ~ICNUdpInterface();
};

} // namespace inet

#endif // ifndef __INET_UDPBASICAPP_H

