import u_are_u as freader

# result = freader.getFinger("base")
#
# print "id: {} \tsize: {}\t len: {}\n".format(result['id'], result['size'], len(result['fmd']))
# reference = freader.getFinger("ref")
# print "id: {}\tsize: {}\t len: {}\n".format(reference['id'], reference['size'], len(reference['fmd']))
#
# print freader.compareFinger(result['fmd'], result['size'], reference['fmd'], reference['size'])

print freader.identifyFinger([5,5,6,7,8,7])
