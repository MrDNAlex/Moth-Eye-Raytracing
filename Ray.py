import numpy as np
from Segment import Segment

class Ray:
    
    Origin : np.ndarray
    
    Direction : np.ndarray
    
    MaxBounce = 5
    
    def __init__ (self, origin, direction):
        
        self.Origin = origin
        self.Direction = direction / np.linalg.norm(direction) 
        
    def Cross (self, v1, v2):
        return v1[0]*v2[1] - v1[1]*v2[0]
    
    def Intersect(self, seg):
        segment = seg.B - seg.A
        
        denom = self.Cross(self.Direction, segment)
        
        if (np.abs(denom) <= 1e-12):
            return False, 0
        
        invDenom = 1 / denom
        aToOrigin = seg.A - self.Origin
        
        t = self.Cross(aToOrigin, segment) * invDenom
        s = self.Cross(aToOrigin, self.Direction) * invDenom
        
        if (t < 1e-12 or (s < 0 or s > 1)):
            return False, 0
        
        return True, t
    
    def GetIntersectPos(self, t):
        return self.Origin + t * self.Direction
    
    def Render(self, ax, label):
        ax.quiver(self.Origin[0], self.Origin[1], self.Direction[0], self.Direction[1],
          angles='xy', scale_units='xy', scale=1, label = label)
        
    def Travel(self, segments):
        
        minT = np.inf
        minSeg = None
        
        for seg in segments:
            hit, t = self.Intersect(seg)
            
            if hit and t < minT:
                minT = t
                minSeg = seg
                
        if minT != np.inf:
            return True, minT, minSeg
        
        return False, 0, None
    
    def Reflect(self, segment):
        
        hit, t = self.Intersect(segment)
        newO = self.GetIntersectPos(t)
        
        D = self.Direction
        N = segment.GetNormal()
        
        D = D / np.linalg.norm(D)
        N = N / np.linalg.norm(N)
        
        newDir = D - 2 * np.dot(D, N) * N
        
        self.Origin = newO
        self.Direction = newDir / np.linalg.norm(newDir)
    
    def Transmit(self, segment, n1, n2):
        
        hit, t = self.Intersect(segment)
        newO = self.GetIntersectPos(t)
        
        normal = segment.GetNormal()
        
        if np.dot(self.Direction, normal) > 0:
            normal = -normal
            
        cos_i = -np.dot(normal, self.Direction)
        sin_i = np.sqrt(max(0.0, 1.0 - cos_i**2))
        
        sin_t = n1 / n2 * sin_i
        
        if sin_t > 1.0:
            return None  # no transmission
        
        cos_t = np.sqrt(1.0 - sin_t**2)
        
        refr_dir = (n1/n2) * self.Direction + (n1/n2 * cos_i - cos_t) * normal
        refr_dir /= np.linalg.norm(refr_dir)
        
        self.Origin = newO
        self.Direction = refr_dir
        
        
        
        
        
        
        