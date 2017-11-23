import u_are_u as freader

result = freader.getFinger("base")

print "id: {} \tsize: {}\t len: {}\n".format(result['id'], result['size'], len(result['fmd']))
reference = freader.getFinger("ref")
print "id: {}\tsize: {}\t len: {}\n".format(reference['id'], reference['size'], len(reference['fmd']))

freader.compareFinger(
[result['fmd'], result['size']], result['size'],
[reference['fmd'], reference['size']], reference['size'])
