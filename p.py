from PIL import Image, ImageOps
import time

name = ""
def rotate(v = 30):
	v= getInt(v)
	image = Image.open(name)
	image = image.rotate(v, expand=True)
	image.save(name)


def solarize(v = 30):
	v= getInt(v)
	image = Image.open(name)
	image = ImageOps.solarize(image, threshold=v)  # default
	image.save(name)

def posterize(v = 1):
	v= int(v)
	image = Image.open(name)
	image = ImageOps.posterize(image, bits=v)
	image.save(name)

def invert(v = 30):
	v= getInt(v)
	image = Image.open(name)
	image = ImageOps.invert(image)
	image.save(name)

def expand(v = 30):
	v= int(v)
	image = Image.open(name)
	image = ImageOps.expand(image, border=v)
	image.save(name)



def getInt(v):
	v= float(v)
	print(v)
	return v


import sys

# print(sys.argv)

if (len(sys.argv)  > 2):

	try:
		print(sys.argv)
		sys.argv = [ str(x).replace("\n","") for x in sys.argv]
		method_to_call = str(sys.argv[1])
		name = str(sys.argv[0])
		method_name = method_to_call
		method = eval(method_name)

		print("called")
		print(sys.argv)
		parameters = [x for x in sys.argv[2:]]

		kwargs = {}
		method(*parameters, **kwargs)
	except Exception as e:
		print(e)
# exit(0)