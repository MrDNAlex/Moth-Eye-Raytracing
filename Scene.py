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

        for ray in self.Rays:

            ray.Render(ax, f"Ray {index}")

            for i in range(ray.MaxBounce):
                
                hit, t, seg = ray.Travel(self.Segments)

                if hit:
                    print(i, t)
                    ax.scatter(*ray.GetIntersectPos(t), color='orange', s=60, label='hit')
                    ray.Transmit(seg, 1, 1.40)
                    ray.GetFresnelCoeffs(seg, 1, 1.40)
                    #ray.Reflect(seg)
                    ray.Render(ax, f"Ray {index} {i}")
                    seg.RenderNormal(ax)

            index += 1
            
        ax.set_aspect('equal')
        ax.grid(True)
        ax.legend()
        ax.set_xlabel('X')
        ax.set_ylabel('Y')
        plt.show()
