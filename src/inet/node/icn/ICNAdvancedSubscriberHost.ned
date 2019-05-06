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

package inet.node.icn;

import inet.node.icn.ICNSubscriberHost;
import inet.physicallayer.ieee80211.packetlevel.Ieee80211ScalarRadioMedium;

// This extends the subscriber Host definition by another
// wireless card that is configured for local communication
module ICNAdvancedSubscriberHost extends ICNSubscriberHost {
    parameters:
		// 2 wireless cards
	    numWlanInterfaces = 2;
		
		// the first one of the wireless cards will operate with default
		// parameters
		// the second one will operate with csmacamac

       	wlan[1].typename = "WirelessInterface";
        wlan[1].radio.typename = "Ieee80211ScalarRadio";
        wlan[1].mac.typename = "CsmaCaMac";
        wlan[1].mac.useAck = false;
        wlan[1].radio.bandName = "5 GHz";
        wlan[1].radio.transmitter.carrierFrequency = 5.0GHz;
        // this way it is possible to reduce the communication range of a host
        //wlan[1].radio.transmitter.power = 2mW;

		// application
		app[0].hasLocalCommunicator = true;
}
