
from PIL import Image
import time

name = ""
def rotate(nn = 30):
	nn= float(nn)
	print(nn)
	image = Image.open(name)
	image.save(name)
	image = image.rotate(nn, expand=True)
	image = image.convert("L")
	image.save("old_{}".format(name))

def say_hello(name):
    print 'Hello {}!'.format(name)



import sys

print(sys.argv)
print(sys.argv)

if (len(sys.argv)  > 2):

	method_to_call = str(sys.argv[1])
	name = str(sys.argv[0])
	method_name = method_to_call
	method = eval(method_name)

	parameters = [x for x in sys.argv[2:]]

	kwargs = {}
	method(*parameters, **kwargs)
# exit(0)