from PIL import Image
from pprint import pprint
import sys
import os
if len(sys.argv) < 3:
	print("Usage: python image2bytes.py infile outfile")
	sys.exit(1)


image_filepath = sys.argv[1]

print('image file:', image_filepath)
myimage = Image.open(image_filepath)

width, height = myimage.size
print(f'size: {width} x {height}')
pixels = []
for y in range(height):
	for x in range(width):
		for i in myimage.getpixel((x, y)):
			pixels.append(i)


c_name = image_filepath.replace('.', '_').replace('-', '_')

outs = f'unsigned char {c_name}_data = {{ {str(pixels).replace("[", "").replace("]", "")} }};\nunsigned int {c_name}_length = {len(pixels)};\nunsigned int {c_name}_width = {width};\nunsigned int {c_name}_height = {height};\n'

with open(sys.argv[2], 'w') as f:
	f.write(outs)
