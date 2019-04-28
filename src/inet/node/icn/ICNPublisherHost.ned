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

import inet.node.icn.ICNRouterHost;

module ICNPublisherHost extends ICNRouterHost {
	parameters:
	    @display("i=device/pc2");
	   	// delay until first message is sent and if repeatPublications is set to true the delay between two messages)
	    int messageDelay @unit(s);
	    // state if publication should be repeated
	    bool repeatPublications;
	    // the name of the data that is being published (see ICNName.h for format)
	    string icnPublicationName;
	    // the size of the data that is being published in bytes
	    int publicationSize @unit(B);
	    
	    app[0].hasPublisher = true;
	    app[0].publisher.repeat = repeatPublications;
	    app[0].publisher.delay = messageDelay;
	    app[0].publisher.dataName = icnPublicationName;
	    app[0].publisher.dataSize = publicationSize;
}