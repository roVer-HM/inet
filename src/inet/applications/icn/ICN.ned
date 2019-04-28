//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package inet.applications.icn;

import inet.applications.icn.ICNBase;
import inet.applications.contract.IApp;

module ICN like IApp
{
    parameters:
        string interfaceTableModule;   // The path to the InterfaceTable module
        bool hasPublisher = default(false);
        // when this is set to false the node will not store subscriptions in its router
        // therefore setting publisher to true and forwarding to false is pointless
        // because the published packet will not be forwarded
        bool icnForwarding = default(false); 
        bool hasSubscriber = default(false);
       	icnBase.routerModule = default(absPath(".router"));
    gates:
        // needed for IApp. The data from lower layers will flow through these gates
        input socketIn;
        output socketOut;
    submodules:
        icnBase: ICNBase {
    		parameters:
    		    interfaceTableModule = interfaceTableModule;
    		    icnForwarding = icnForwarding;
    		    hasPublisher = hasPublisher;
    		    hasSubscriber = hasSubscriber;
    	}
    	publisher: ICNPublisher if hasPublisher;
    	router: ICNRouter;
    	subscriber: ICNSubscriber if hasSubscriber;
    connections allowunconnected:
        // socket connections to icn base. icn base handles sending and receiving packets
		socketIn --> icnBase.socketIn;
		socketOut <-- icnBase.socketOut;
		
		// Publisher
		// ------------------------------
		
		// publications will flow from publisher to icnBase who will broadcast them
		publisher.publications --> icnBase.messageInterface++ if hasPublisher;
		
		// Subscriber
		// ------------------------------
		
		// subscriptions will flow from subscriber to icnBase who will broadcast them
		subscriber.subscriptions --> icnBase.messageInterface++ if hasSubscriber;
		
		// transports packets that this node subscribed to to the subscriber (application data)
		icnBase.subscribedPackets --> subscriber.messageInterface++ if hasSubscriber;
		
		// transports notifications about heartbeats from icn base to subscriber
		icnBase.receivedHeartbeats --> subscriber.messageInterface++ if hasSubscriber;
		
}
