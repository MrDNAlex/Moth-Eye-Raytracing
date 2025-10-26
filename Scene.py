from Ray import Ray
from Segment import Segment
import numpy as np
import matplotlib.pyplot as plt

class Scene:

    Rays: list[Ray]

    Segments: list[Segment]

    def __init__(self):
        self.Rays = []
        self.Segments = []

    def AddSegment(self, segment):
        self.Segments.append(segment)

    def AddRay(self, ray):
        self.Rays.append(ray)

    def Render(self):
        fig, ax = plt.subplots()

        index = 0

        # Render Segments
        for seg in self.Segments:
            seg.Render(ax)

        while len(self.Rays) > 0:

            newRays = []
            
            for ray in self.Rays:

                ray.Render(ax, f"Ray {index}")
                
                rays = ray.Travel(self.Segments)
                
                if rays != None:
                    newRays.extend(rays)
                
            self.Rays = newRays
            index += 1
            
        ax.set_aspect('equal')
        ax.grid(True)
        ax.legend()
        ax.set_xlabel('X')
        ax.set_ylabel('Y')
        plt.show()
