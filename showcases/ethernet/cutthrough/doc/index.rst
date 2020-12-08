Ethernet Cut-Through Switching
==============================

Goals
-----

.. so

  - cut-through switching can reduce latency of ethernet frames
  - works best when the frame goes through a lot of switches
  - cut-through switching (start forwarding the frame on another interface as soon as the header is received, and the next-hop address is available, as opposed to receive the whole packet and forward after that)
  - the new composable/layered ethernet model supports packet streams and cutthrough switching

Cut-through switching can reduce switching delay of ethernet frames by immediatelly forwarding an ethernet frame after the header is received and the switch knows which outgoing interface to send the frame on (as opposed to store-and-forward switching, in which the whole frame is received and then forwarded).

.. **TODO** which outgoing interface to use/to send the frame on

.. This showcase demonstrates cut-through switching, and compares its delay with store-and-forward switching.

This showcase demonstrates cut-through switching, and compares it to store-and-forward switching in terms of delay.

.. The new composable/layered Ethernet model supports cut-through switching.

The Model
---------

.. - cut through switching reduces delay
   - it skips the FCS check (its handles in the endpoints)
   - it requires packet streams (also enables preemption)
   - need LayeredEthernetInterface, and CuttroughEthernetInterface in switches

.. **TODO** works best when the frame goes through a lot of switches

Cut-through switching reduces switching delay, but skips the FCS check, as the FCS is at the end of the Ethernet frame; the FCS check is performed in at destination host. The delay reduction is more substantial if the packet goes through multiple switches (as one packet transmission duration can be saved at each switch).

Cut-through switching makes use of intra-node  packet streaming in INET's modular Ethernet model.
Packet streaming is required because the frame needs to be processed as a stream (as opposed to as a whole packet) in order for the switch to be able to start forwarding it before the whole packet is received.

.. **TODO** store-and-forward is the default

.. note:: The default is store-and-forward behavior in hosts such as :ned:`StandardHost`.

The example simulation contains two :ned:`StandardHost` nodes connected by two :ned:`EthernetSwitch`' nodes (all connections are 1 Gbps):

.. figure:: media/Network.png
   :align: center
   :width: 100%

.. **TODO** 1Gbps connection

.. There are two configurations in omnetpp.ini. In both of them, host1 sends UDP packets to host2.

In the simulation, host1 sends 1000-Byte UDP packets to host2, with a mean arrival time of 100ms, and X ms jitter. There are two configurations in omnetpp.ini, ``NoCuttrough`` and ``Cuttrough`` (only differring in the use of cuttrough switching).

**TODO** NoCuttrough -> StoreAndForward; - in config name (try)

.. In the ``General`` configuration,

.. Here is the part from the ``General`` configuration concerning Ethernet:

In the ``General`` configuration, the following lines configure the hosts and switches to use the modular ethernet model:

**TODO** typename = DropTailQueue

**TODO** ini-be -> NoCuttrough -> #default behavior, no configuration required

.. literalinclude:: ../omnetpp.ini
   :start-at: *.*.encap.typename
   :end-at: LayeredEthernetInterface
   :language: ini

The encapsulation type :ned:`OmittedEthernetEncapsulation` is there for backward compatibility purposes;
it's a dummy encapsulation module required by the new layered ethernet model so that it is a drop-in replacement for the old one).

.. **TODO** dummy encapsulation/decapsulation module required by the new modular/layered ethernet model (its needed so that the new model is a drop-in replacement for the old one)

Also, the link speed is specified:

.. literalinclude:: ../omnetpp.ini
   :start-at: bitrate
   :end-at: bitrate
   :language: ini

To prevent packets from accumulating in the queue of the sender host (and thus increasing the measured end-to-end delay), the queue is limited to one packet, and configured to drop packets from the end of the queue:

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

Here is a video of the cut-through behavior in Qtenv:

**TODO** store and forward video; disable arp

.. video:: media/cuttrough.mp4
   :width: 100%
   :align: center

.. note:: The ARP request is broadcast, so it is not utilizing cut-through.

The following sequence chart excerpt shows a packet sent from host1 to host2 via the switches:

**TODO** linear time

**TODO** seq chart store and forward

**TODO** azert vannak kiugrok mert 1 csomag tud varakozni a queue-ban; vagy change the rate (ritkabban)

.. figure:: media/seqchart.png
   :align: center
   :width: 100%

We compared the end-to-end delay of the UDP packets in the case of store-and-forward swtiching vs cut-through switching:

.. figure:: media/delay.png
   :align: center
   :width: 100%

**TODO** szamolgatas; n*transmission duration + 3 propagation time; 1 transmission + 3 propagation + 2 cuttrough (header reception)(cuttrough delay)

1054B; 8.432us; 25.296+propagation time

(1000 + 8 + 20 + 18 + 8) * 8 / 1E+9 * 3 / 1E-6
(1000 + 8 + 20 + 18 + 8) * 8 / 1E+9 / 1E-6 + 22 / 1E+9 / 1E-6 * 2
