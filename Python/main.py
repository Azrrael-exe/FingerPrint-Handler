from py_uareu import u_are_u as freader

fingers = []
for i in xrange(5):
    fingers.append(freader.getFinger("finger# {}".format(i)))

ref = freader.getFinger("ref")
# result = freader.getFinger("result")
#
# print "id: {} \tsize: {}\t len: {}\n".format(result['id'], result['size'], len(result['fmd']))
# print "id: {}\tsize: {}\t len: {}\n".format(ref['id'], ref['size'], len(ref['fmd']))
# print freader.compareFinger(result['fmd'], result['size'], ref['fmd'], ref['size'])

# Make the Input
def makeCompareInput(finger, fingers):
    out = []
    out.append(finger['fmd'])
    out.append(finger['size'])
    for f in fingers:
        out.append(f['fmd'])
        out.append(f['size'])
    return tuple(out)

comp = makeCompareInput(ref, fingers)

print freader.identifyFinger(comp)
