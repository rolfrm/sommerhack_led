import numpy
import csv

def read_coordinates_file(path):
    coords = []
    with open(path, 'rb') as f:
        reader = csv.reader(f, delimiter = ",")
        for row in reader:
            coords.append([float(row[0]), float(row[1])])
    return coords
