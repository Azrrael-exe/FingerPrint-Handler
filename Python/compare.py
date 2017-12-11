from py_uareu import u_are_u as freader
from pymongo import MongoClient
import bson.binary
from bson.codec_options import CodecOptions

client = MongoClient()
db = client['fprint']
fingers = db['finger']

finger_list = []
candidates = []

for i in xrange(20):
    for finger in fingers.find({}):
        finger_list.append(finger['fmd'].decode('hex'))
        finger_list.append(finger['size'])
        candidates.append(finger)

print len(finger_list)

ref = freader.getFinger("ref")

comp = [ref['fmd'], ref['size']] + finger_list

result = freader.identifyFinger(tuple(comp))

print result

if(result['candidates']):
    print candidates[result['index']]['id']
else :
    print "No candidates"
