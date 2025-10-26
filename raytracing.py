import numpy as np
import matplotlib.pyplot as plt
from Ray import Ray
from Scene import Scene
from Segment import Segment

def normalize(v):
    n = np.linalg.norm(v)      # length of the vector
    if n == 0:
        return v               # avoid division by zero
    return v / n

scene = Scene()

O = np.array([0.2, 0.4])
D = np.array([0.8, 0.2])

ray = Ray(O, D)

A = np.array([1, 2])
B = np.array([2, 1.5])

x = np.linspace(1, 10, 50)
y = np.sin(x)

for i in range(len(x) - 1):
    segment = Segment(x[i], y[i], x[i+1], y[i+1])
    scene.AddSegment(segment)
    
y = np.sin(x) + 2

for i in range(len(x) - 1):
    segment = Segment(x[i], y[i], x[i+1], y[i+1])
    scene.AddSegment(segment)
    
scene.AddSegment(Segment(1.0, 1.0, 10.0, 1.0))

scene.AddRay(Ray(np.array([1, 2]), np.array([1, -0.8])))

scene.Render()
