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

import inet.node.icn.ICNPublisherHost;

module ICNAccessPoint extends ICNPublisherHost {
	parameters:
	    @display("i=device/accesspoint");
	    // specifies the id (name) of this access point
	    string id;
	    
	    // specify values of publisher host
	    messageDelay = 2s;
	    repeatPublications = true;
	    icnPublicationName = "/accessPoint/heartbeat/" + id;
	    publicationSize = 10B;
	    
	    // we need to broadcast 
	    app[0].publisher.broadcast = true;
}