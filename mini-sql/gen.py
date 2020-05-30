f = open("dbcache.json", "w")
f.write("[")
f.write('{"balance":0, "_id":0}')
for i in range(10**6):
    f.write(', {"balance":%d, "_id":%d}' % (i//4, i))
f.write("]")