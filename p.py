from PIL import Image
import time


def rotateImg(name):
	image = Image.open("img.jpg")
	image = image.rotate(30, expand=True)
	image = image.convert("L")
	image.save("{}.jpg".format(name))

def say_hello(name):
    print 'Hello {}!'.format(name)



import sys

print(sys.argv)

if (len(sys.argv)  > 2):
	name = str(sys.argv[1])
	method_to_call = str(sys.argv[2])


	method_name = method_to_call
	method = eval(method_name)

	args = [name]
	kwargs = {}
	method(*args, **kwargs)