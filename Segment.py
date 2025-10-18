import numpy as np

class Segment:
    
    A : np.ndarray
    
    B : np.ndarray
    
    def __init__(self, x1, y1, x2, y2):
        self.A = np.array([x1, y1])
        self.B = np.array([x2, y2])
    
    def Render(self, ax):
        ax.plot([self.A[0], self.B[0]], [self.A[1], self.B[1]], color="blue", linewidth=2)
        
    def RenderNormal(self, ax):
        
        normal = self.A + self.GetNormal(False)
        
        ax.plot([self.A[0], normal[0]], [self.A[1], normal[1]], color="blue", linewidth=2)
    
    def GetNormal(self, left=True):
        """
        Returns the normalized 2D normal vector of this segment.
        If left=True, rotates 90° counter-clockwise (left-hand normal),
        else clockwise (right-hand normal).
        """
        e = self.B - self.A                # segment direction
        
        if np.allclose(e, 0):
            return np.array([0.0, 0.0])    # degenerate segment

        # Rotate 90°: (-y, x) for left normal, (y, -x) for right normal
        n = np.array([-e[1], e[0]]) if left else np.array([e[1], -e[0]])
        n /= np.linalg.norm(n)
        return n

    