import numpy as np
from Segment import Segment

class Ray:
    
    Origin : np.ndarray
    
    Direction : np.ndarray
    
    CurrentMedium : float
    
    CurrentBounce : int
    
    MaxBounce = 7
    
    Power : float
    
    def __init__ (self, origin, direction, currentBounce = 0, power = 1):
        
        self.Origin = origin
        self.Direction = direction / np.linalg.norm(direction) 
        self.CurrentMedium = 1
        self.Power = power
        self.CurrentBounce = currentBounce
      
    def Clone (self):
        return Ray(self.Origin, self.Direction, self.CurrentBounce, self.Power)
        
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
        
    def Travel(self, segments: list[Segment]):
        
        self.CurrentBounce += 1
        
        if self.CurrentBounce > self.MaxBounce:
            return None
        
        minT = np.inf
        minSeg = None
        
        for seg in segments:
            hit, t = self.Intersect(seg)
            
            if hit and t < minT:
                minT = t
                minSeg = seg
        if minT == np.inf:
            return None
        
        R, T = self.GetFresnelCoeffs(minSeg, 1, 1.40)
        
        cloneRay = self.Clone()
        
        rays = []
        
        self.Power *= R
        cloneRay.Power *= T
        
        if (self.Power > 0.005):
            self.Reflect(minSeg)
            rays.append(self)
        else:
            print("Ray destroyed")
        
        if (cloneRay.Power > 0.005):
            cloneRay.Transmit(minSeg, 1, 1.4)
            rays.append(cloneRay)
        else:
            print("Ray destroyed")
    
        return rays
        
    def Reflect(self, segment : Segment):
        
        hit, t = self.Intersect(segment)
        newO = self.GetIntersectPos(t)
        
        D = self.Direction
        N = segment.GetNormal()
        
        D = D / np.linalg.norm(D)
        N = N / np.linalg.norm(N)
        
        newDir = D - 2 * np.dot(D, N) * N
        
        self.Origin = newO
        self.Direction = newDir / np.linalg.norm(newDir)
    
    def Transmit(self, segment: Segment, n1, n2):
        
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
        
    def GetFresnelCoeffs(self, segment : Segment, n1, n2):
        
        normal = -1*segment.GetNormal()
        
        incidentCos = -np.dot(segment.GetNormal(), self.Direction)
        incidentSin = np.sqrt(max(0.0, 1.0 - incidentCos**2))
        
        transmitSin = n1 / n2 * incidentSin
        transmitCos = np.sqrt(max(0.0, 1.0 - transmitSin**2))
        
        n1ICos = n1*incidentCos
        n2ICos = n2*incidentCos
        
        n1TCos = n1*transmitCos
        n2TCos = n2*transmitCos
        
        inverseS = 1/(n1ICos+n2TCos)
        inverseP = 1/(n2ICos+n1TCos)
        
        rs = (n1ICos-n2TCos)*inverseS
        ts = (2*n1ICos)*inverseS

        rp = (n2ICos-n1TCos)*inverseP
        tp = (2*n1ICos)*inverseP
        
        Rs = rs**2
        Rp = rp**2
        
        R = 0.5 *(Rs + Rp)
        T = 1 - R

        return R, T
        