import numpy as np

class Segment:
    
    A : np.ndarray
    
    B : np.ndarray
    
    def __init__(self, x1, y1, x2, y2):
        self.A = np.array([x1, y1])
        self.B = np.array([x2, y2])
    
    def Render(self, ax):
        ax.plot([self.A[0], self.B[0]], [self.A[1], self.B[1]], color="blue", linewidth=2)
    