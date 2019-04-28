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

import inet.node.inet.WirelessHost;
import inet.applications.icn.ICN;

module ICNHostBase extends WirelessHost {
	parameters:
	    numApps = 1;
	    ipv4.arp.typename = "GlobalArp";
	    app[0].typename = "ICN";
	    wlan[0].typename = "WirelessInterface";
	    wlan[0].radio.typename = "UnitDiskRadio";
	    wlan[0].mac.typename = "CsmaCaMac";
	    wlan[0].radio.receiver.ignoreInterference = false;
        wlan[0].radio.transmitter.interferenceRange = 300m;
        wlan[0].mac.ackTimeout = 300us;
        **.bitrate = 10Mbps;
        wlan[0].radio.transmitter.communicationRange = 280m;
        wlan[0].mac.useAck = false;
}