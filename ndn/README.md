DTN/NDN experiment
==========


FILES:
-----

- **autoreg.sh**: used to keep the name _/sn_ registered in the FIB
- **dtn-startegy**: forwarding strategy used in the experiment. The idea is to send another Interest before the previous one expires. The **dtn-startegy.cpp** and **dtn-startegy.hpp** should live inside NFD/daemon/fw/dtn-strategy
- **dtn-consumer.cpp**: is a consumer adapted to this experiment.
- **producer.cpp**: classic producer


EXPERIMENT:
----------

- Put the files in their corresponding locations
- Rebuild NFD, NDN, etc.
- Start NFDs
- Apply the forwarding strategy to the namespace _/sn_
- Run producer (destination station, contains Data), then consumer (source station, initiates Interest)