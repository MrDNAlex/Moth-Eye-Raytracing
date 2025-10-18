import numpy as np

class Ray:
    
    Origin : np.ndarray
    
    Direction : np.ndarray
    
    def __init__ (self, origin, direction):
        
        self.Origin = origin
        self.Direction = direction
        
    def Cross (self, v1, v2):
        return v1[0]*v2[1] - v1[1]*v2[0]
    
    def Intersect(self, seg):
        segment = seg.B - seg.A
        
        denom = self.Cross(self.Direction, segment)
        
        if (np.abs(denom) <= 1e-12):
            return False, np.array([0, 0])
        
        invDenom = 1 / denom
        
        t = self.Cross((seg.A - self.Origin), segment) * invDenom
        s = self.Cross((seg.A - self.Origin), self.Direction) * invDenom
        
        if (t < 0 or (s < 0 or s > 1)):
            return False, np.array([0, 0])
        
        intersection = self.Origin + t * self.Direction
        
        return True, intersection
    
    def Render(self, ax, label):
        ax.quiver(self.Origin[0], self.Origin[1], self.Direction[0], self.Direction[1],
          angles='xy', scale_units='xy', scale=0.1,
          color='red', label = label)
    
        
        
        
        
        