import numpy as np
import matplotlib.pyplot as plt

def normalize(v):
    n = np.linalg.norm(v)      # length of the vector
    if n == 0:
        return v               # avoid division by zero
    return v / n

def cross (A, B):
    return A[0]*B[1] - A[1]*B[0]


O = np.array([0.2, 0.4])
D = normalize(np.array([0.6, 0.4]))

A = np.array([1, 2])
B = np.array([2, 1.5])
e = normalize(B - A)

denom = cross(D, e)
invDenom = 1/denom

t = cross((O-A), e)
s = cross((O-A), D)

print(t)
print(s)

intersec = O + t*D



fig, ax = plt.subplots()

# Plot ray
ax.quiver(O[0], O[1], D[0], D[1],
          angles='xy', scale_units='xy', scale=1,
          color='red', label='Ray')

# Plot segment
ax.plot([A[0], B[0]], [A[1], B[1]],
        color='blue', linewidth=2, label='Segment')

# Plot points
ax.scatter(*O, color='red', s=60, label='O (origin)')
ax.scatter(*A, color='green', s=60, label='A')
ax.scatter(*B, color='orange', s=60, label='B')

ax.scatter(*intersec, color='purple', s=60, label='Intersection')

# Equal aspect ratio so geometry looks right
ax.set_aspect('equal')
ax.grid(True)
ax.legend()
ax.set_xlim(-0.5, 3)
ax.set_ylim(-0.5, 3)
ax.set_xlabel('X')
ax.set_ylabel('Y')
plt.show()