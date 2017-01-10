import sys
import time
from pyndn import Name
from pyndn import Face

from flask import Flask, request
from twilio import twiml

app = Flask(__name__)

RESPONSE = ""
def update_response(resp):
    global RESPONSE
    RESPONSE = resp

def dump(*list):
    result = ""
    for element in list:
        result += (element if type(element) is str else str(element)) + " "
    print(result)

class Counter(object):
    def __init__(self):
        self._callbackCount = 0

    def onData(self, interest, data):
        self._callbackCount += 1
        dump("Got data packet with name", data.getName().toUri())
        # Use join to convert each byte to chr.
        dump(data.getContent().toRawStr())
        update_response(data.getContent().toRawStr())
    def onTimeout(self, interest):
        self._callbackCount += 1
        dump("Time out for interest", interest.getName().toUri())
        update_response("timeout")

def send_interest(prefix):
    # The default Face will connect using a Unix socket, or to "localhost".
    face = Face()

    counter = Counter()
    name = Name(prefix)
    dump("Express name ", name.toUri())
    face.expressInterest(name, counter.onData, counter.onTimeout)

    while counter._callbackCount < 1:
        face.processEvents()
        # We need to sleep for a few milliseconds so we don't use 100% of the CPU.
        time.sleep(0.01)

    face.shutdown()

@app.route('/sms', methods=['POST'])
def sms():
    number = request.form['From']
    message_body = request.form['Body']
    send_interest(message_body)
    resp = twiml.Response()
    if RESPONSE == "timeout":
        resp.message('Interest timed out.')
    else:
        resp.message('Data: {}'.format(RESPONSE))
    return str(resp)

if __name__ == '__main__':
    app.run()
