

package inet.applications.tcpapp;

import inet.applications.contract.IApp;

//
// EventNotificationPublisher
// 
// Will accept all incoming tcp connections and when its time it will send messages
// to all connected clients.
//
simple EventNotificationPublisher like IApp
{
    parameters:
        // my params:
        // ----------
        string publicationName; // will be expanded with "/[simtime at publication]"
        // if this is true the publication will be repeated
    	bool repeat = default(false);
        // the delay between two messages (only has an effect if repeat is set to true)
		int delay @unit(s);
		// the size of the data
		int dataSize @unit(B) = default(500B);
		// the sim time to start at
		int startTime @unit(s);
		
		// old params:
		// ----------
        string localAddress = default(""); // local address; may be left empty ("")
        int localPort = default(5555);     // localPort number to listen on
        @display("i=block/app");
        @lifecycleSupport;
    gates:
        input socketIn @labels(TcpCommand/up);
        output socketOut @labels(TcpCommand/down);
}

