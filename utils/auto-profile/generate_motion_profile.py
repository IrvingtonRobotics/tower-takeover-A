#!/usr/bin/env python3

import re
import math
import cmath

def generate(body, scale):
    groups = solve_groups(body, scale)
    return out_format(groups)

def solve_groups(body, scale):
    matches = re.findall(r'<circle [^>]*?fill-opacity="0.98"[^>]*/>', body)
    points = []
    points_complex = []
    angles = []
    float_re = r'-?\d+(?:\.\d+)?'
    for full in matches:
        matrix_match = re.search(fr'matrix\(((?:{float_re} ){{5}}{float_re})\)', full)
        if matrix_match:
          parts = [float(s) for s in matrix_match.group(1).split(" ")]
          # range of acos from 0 to +pi
          theta = math.acos(parts[0]) * 180/math.pi
          if parts[1] < 0:
            # negative sin, so mirror to negative range
            theta = -theta
          x = parts[4]
          y = parts[5]
        else:
          x_match = re.search(fr'cx="({float_re})"', full)
          x = float(x_match.group(1)) if x_match else 0
          y_match = re.search(fr'cy="({float_re})"', full)
          y = float(y_match.group(1)) if y_match else 0
          # no close paren because a centerpoint is indicated on rotate
          theta_match = re.search(fr'rotate\(({float_re})', full)
          theta = float(theta_match.group(1)) if theta_match else 0
        x /= scale
        y /= scale
        points.append((x, y))
        pos = complex(f"{x}+{y}j")
        points_complex.append(pos)
        angles.append(theta)
    return groups(points, points_complex, angles)

def similar_point(p1, p2):
    tol = 0.01
    return abs(p2[0]-p1[0]) < tol and abs(p2[1]-p1[1]) < tol


def groups(points, points_complex, angles):
    groups = [(False, [])]
    def add_pos(point, angle):
        groups[-1][1].append((*point, angle))

    reversing = False
    for pt, point, angle, next_point in zip(points, points_complex, angles, points_complex[1:]):
        if len(groups[-1][1]) > 0 and similar_point(groups[-1][1][-1], pt):
          groups.append((f"drive.turnAngle({f(angle - groups[-1][1][-1][-1])});",))
          groups.append((reversing, [groups[-2][1][-1]]))
        else:
          add_pos(pt, angle)
        move_angle_degrees = cmath.phase(next_point - point) * 180/math.pi
        diff = move_angle_degrees - angle
        # regret not using dotproduct
        diff %= 360
        if (90 < diff < 270) ^ reversing:
            reversing = not reversing
            groups.append((reversing, [groups[-1][1][-1]]))
    add_pos(points[-1], angles[-1])
    return groups

def f(x):
  return round(x, 4)

def out_format(groups):
    indent = "  "
    s = ""
    for group in groups:
        if len(group) == 1:
          s += group[0] + "\n"
          continue
        reversed, positions = group
        m = 1 if reversed else -1
        points = map(lambda x: indent + f"Point{{{f(x[0])}_in, {f(m*x[1])}_in, {f(m*x[2])}_deg}}", positions)
        points_str = ",\n".join(points)
        s += "travelProfile({\n" + points_str + "\n}, " + ("true" if reversed else "false") + ", 0.5);\n"
    return s
