
from PIL import Image
import numpy
import cv2
import time
import sys

name = ""
def rotate(angle = 30):
	angle = float(angle)
	image = Image.open(name)
	image.save(name)
	image = image.rotate(angle)
	image.save("{}".format(name))

def highpass(sigma = 29):
	sigma = int(sigma)
	if(sigma%2):
		image = cv2.imread(name)
		blur = cv2.GaussianBlur(image, (sigma,sigma),0)
		image = image - blur
		image = image + 127
		cv2.imwrite(name, image)
	else:
		print("Sigma trebuie sa fie numar impar!")
		sys.exit()

def lowpass(sigma = 29):
	sigma = int(sigma)
	if(sigma%2):
		image = cv2.imread(name)
		image = cv2.GaussianBlur(image,(sigma,sigma),0)
		cv2.imwrite(name,image)
	else:
		print("Sigma trebuie sa fie numar impar!")
		sys.exit()

def hsv():
	image = cv2.imread(name)
	image = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)
	cv2.imwrite(name,image)

def neg():
	image = cv2.imread(name)
	image = 255-image
	cv2.imwrite(name,image)

def bnw():
	image = cv2.imread(name)
	image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
	(thresh, image) = cv2.threshold(image, 127,255,cv2.THRESH_BINARY)
	cv2.imwrite(name,image)



print(sys.argv)

if (len(sys.argv)  > 2):
	method_to_call = str(sys.argv[4])
	name = str(sys.argv[3])
	image = Image.open(name)
	image.save("old_" + name)
	method_name = method_to_call
	method = eval(method_name)

	parameters = [x for x in sys.argv[5:]]
	kwargs = {}
	method(*parameters, **kwargs)