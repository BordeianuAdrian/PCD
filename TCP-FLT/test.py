from PIL import Image

name = "florin.fortis.jpg"
image = Image.open(name)
image = image.save("old_" + name)
image = image.rotate(30, expand=True)
image = image.convert("L")
image.save(name)