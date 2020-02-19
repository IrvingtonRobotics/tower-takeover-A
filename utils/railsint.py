#!/usr/bin/env python3
import numpy as np
from scipy import integrate

cubes = 6
# mu = tan(theta) where theta is the angle until the cubes slide
mu = 0.233
# m/s^2
g = 9.81
# m
h = cubes * 5.5 * 0.0254
# rad from ground
forward_angle = np.pi/2
back_angle = forward_angle-2500/1800/5*2*np.pi
# back_angle = 0
# rad/s
start_speed = 0

c1 = g/h*np.sqrt(mu*mu+1)
c2 = np.arctan(mu)

print("c1c2", c1, c2)

# https://nathantypanski.com/blog/2014-08-23-ode-solver-py.html
def solvr(Y, t):
  # Y = [theta, theta']
  return [Y[1], -c1*np.cos(Y[0] - c2)]

# integrate in reverse direction to reach 0 velocity at end, not beginning
a_t = np.linspace(0.55, 0, 200)
asol = integrate.odeint(solvr, [forward_angle, start_speed], a_t)
print("t", "theta", "theta_t", "theta_tt", sep="\t")
for i, t in enumerate(a_t[:-1]):
  theta = asol[i][0]
  theta_t = asol[i][1]
  theta_tt = (asol[i+1][1]-asol[i][1])/(a_t[i+1]-a_t[i])
  print(f"{t:.6f}\t{theta:.6f}\t{theta_t:.6f}\t{theta_tt:.6f}")

"""
Learnings:
(Graphed on desmos)
1. Observing a lower cube allows the stackout to proceed faster (we can stack faster with
fewer cubes). This makes sense.
2. coefficient of friction is insignificant for small mu (0.05 to 0.1)
  - wow mu=0.233 hopefully it matters
3. We can't start out with full velocity, so we can't reach these ideal positions
"""
