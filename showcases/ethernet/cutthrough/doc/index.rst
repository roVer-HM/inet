Ethernet Cut-Through Switching
==============================

Goals
-----

so

- cut-through switching can reduce latency of ethernet frames
- works best when the frame goes through a lot of switches
- cut-through switching (start forwarding the frame on another interface as soon as the header is received, and the next-hop address is available, as opposed to receive the whole packet and forward after that)
- the new composable/layered ethernet model supports packet streams and cutthrough switching

Cut-through switching can reduce switching delay of ethernet frames by immediatelly forwarding an ethernet frame after the header is received and the next-hop address is available (as opposed to store-and-forward switching, in which the whole frame is received and then forwarded).

.. This showcase demonstrates cut-through switching, and compares its delay with store-and-forward switching.

This showcase demonstrates cut-through switching, and compares it to store-and-forward switching in terms of delay.

.. The new composable/layered Ethernet model supports cut-through switching.

The Model
---------

.. - cut through switching reduces delay
   - it skips the FCS check (its handles in the endpoints)
   - it requires packet streams (also enables preemption)
   - need LayeredEthernetInterface, and CuttroughEthernetInterface in switches

Cut-through switching reduces switching delay, but skips the FCS check, as the FCS is at the end of the Ethernet frame, the FCS check is performed in at destination host.

Cut-through switching makes use of packet streams in INET's Ethernet model.
Packet streaming is required for cut-through switching, because the frame needs to be represented as a stream (as opposed to as a packet) in order for the switch to be able to start forwarding it before the whole packet is received.

The example simulation contains two :ned:`StandardHost`'s connected by two :ned:`EthernetSwitch`'es (all connections are 1 Gbps):

.. figure:: media/Network.png
   :align: center
   :width: 100%

.. **TODO** 1Gbps connection

There are two configurations in omnetpp.ini. In both of them, host1 sends UDP packets to host2.

In the simulation, host1 sends 1000-Byte UDP packets to host2, around every 100ms. There are two configurations in omnetpp.ini, ``NoCuttrough`` and ``Cuttrough`` (only differring in the use of cuttrough switching).

.. In the ``General`` configuration,

.. Here is the part from the ``General`` configuration concerning Ethernet:

In the ``General`` configuration, the following lines configure the hosts and switches to use the new ethernet model:

.. literalinclude:: ../omnetpp.ini
   :start-at: *.*.encap.typename
   :end-at: LayeredEthernetInterface
   :language: ini

The encapsulation type :ned:`OmittedEthernetEncapsulation` is there for backward compatibility purposes

dummy encapsulation/decapsulation module required by the new modular/layered ethernet model (its needed so that the new model is a drop-in replacement for the old one)

Also, the link speed is specified:

.. literalinclude:: ../omnetpp.ini
   :start-at: bitrate
   :end-at: bitrate
   :language: ini

To prevent packets from accumulating in the queue of the sender host (and thus increasing end-to-end delay), the queue is limited to one packet, and configured to drop packets from the end of the queue:

.. literalinclude:: ../omnetpp.ini
   :start-at: packetCapacity
   :end-at: dropperClass
   :language: ini

Here are the two configurations:

.. literalinclude:: ../omnetpp.ini
   :start-at: NoCuttrough
   :end-at: phyLayer
   :language: ini

.. - the cuttroughinterface is needed for the cuttrough
   - the streamingphylayer is needed also for that cuttrough (it needs to support packet streaming)(the cuttroughinterface does that by default)

The cuttrough interface in the switches support packet streaming by default; the :ned:`EthernetStreamingPhyLayer` in the hosts support packet streaming as well.

Results
-------

.. - qtenv
   - seqchart
   - chart

Here is a video of the cut-trough behavior in Qtenv:

.. video:: media/cuttrough.mp4
   :width: 100%
   :align: center

The ARP request is broadcast, so it is not utilizing cut-through.

The following sequence chart excerpt shows a packet sent from host1 to host2 via the switches:

.. figure:: media/seqchart.png
   :align: center
   :width: 100%

We compared the end-to-end delay of the UDP packets in the case of store-and-forward swtiching vs cut-through switching:

.. figure:: media/delay.png
   :align: center
   :width: 100%
