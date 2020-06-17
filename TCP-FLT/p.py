
from PIL import Image
import time
import sys

def rotateImg(name):
	image = Image.open(name)
	image = image.save("old_" + name)
	image = image.rotate(30, expand=True)
	image = image.convert("L")
	name_modified = name[0:len(name)-4]
	image.save(name)

def say_hello(name):
    print 'Hello {}!'.format(name)

if(len(sys.argv) > 3):
	name = str(sys.argv[3])
	rotateImg(name)


#if (len(sys.argv)  > 2):
#	name = str(sys.argv[1])
#	method_to_call = str(sys.argv[2])
#
#
#	method_name = method_to_call
#	method = eval(method_name)
#
#	args = [name]
#	kwargs = {}
#	method(*args, **kwargs)