from PIL import Image
import numpy as np

path = 'P:/vector/_anki_dumps/preview/cam_dump_0002_00.raw'
data = np.fromfile(path, dtype='uint8')
dimg = np.reshape(data, (-1,320*4))
#data = np.zeros((h, w, 3), dtype=np.uint8)
#data[0:1600, 0:720] = [255, 0, 0] # red patch in upper left

img = Image.fromarray(dimg)
img.show()