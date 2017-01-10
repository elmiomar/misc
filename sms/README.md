# NDN meets SMS

This is an example of sending and Interest as an SMS.

### Rationale

The idea is sending an sms to a node, this node will retrieve the content, which is a simple string that represents an Interest name.
The node will create an Interest from that name and send it to the NDN network. Once it receives the Data back it will forward it via SMS. Again, the Data is small string nothing heavy and **no encoding/adaptation involved for now**.

### Requierments

For this example we use [Twilio](https://www.twilio.com) for SMS handling. Twilio is a cloud communications platform as a service, that allows to programmatically make/receive phone calls and send/receive text messages using its web service APIs.

- Twilio [free trial](https://www.twilio.com/try-twilio)
- Twilio [quickstart](https://www.twilio.com/docs/quickstart/python/sms)


Since we used the python API from Twilio, we also used the Python NDN client library, [PyNDN2](https://github.com/named-data/PyNDN2), for handling Interest. Receiver script [here]().
For the producer, you can use any client library you prefer. We used the [procuder](https://github.com/named-data/ndn-cxx/blob/master/examples/producer.cpp) from the C++ library.

Make sure your ndn-cxx and PyNDN2 are installed and nfd is running. Go to [HOWTO.md](https://github.com/elmiomar/misc/blob/master/sms/HOWTO.md).
