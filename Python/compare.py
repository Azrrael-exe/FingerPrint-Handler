from py_uareu import u_are_u as freader
from pymongo import MongoClient
import bson.binary
from bson.codec_options import CodecOptions

client = MongoClient()
db = client['fprint']
fingers = db['finger']

finger_list = []

for finger in fingers.find():
    finger_list.append(finger['fmd'].decode('hex'))
    finger_list.append(finger['size'])
