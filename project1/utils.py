import os
from PIL import Image
def list_files(path):
    """
    Returns a list of names (with extension, without full path) of all files in the specified folder.
    """
    files = []
    for name in os.listdir(path):
        if os.path.isfile(os.path.join(path, name)):
            files.append(name)
    return files

def crop_to_smallest_edge(image_path, save_path):
    """
    Reads a .png image, crops it to the size of the smallest edge, and saves the cropped image.
    """
    try:
        im = Image.open(image_path)
        width, height = im.size
        min_edge_size = min(width, height)
        cropped_im = im.crop(((width - min_edge_size) / 2, (height - min_edge_size) / 2, (width + min_edge_size) / 2, (height + min_edge_size) / 2))
        cropped_im.save(save_path, "PNG")
        
    except Exception as e:
        print(f"Error processing image: {e}")
        
def makedirs(name):
    os.makedirs(name, exist_ok=True)
    os.makedirs(name + "_train", exist_ok=True)