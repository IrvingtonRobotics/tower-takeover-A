#!/usr/bin/env python3

import re
import math
import cmath

def generate(body, scale):
    groups = solve_groups(body, scale)
    return out_format(groups)

def solve_groups(body, scale):
    matches = re.findall(r'(<circle cx="(\d+(?:\.\d+)?)" cy="(\d+(?:\.\d+)?)[^>]*?fill-opacity="0.98"[^>]*/>)', body)
    points = []
    points_complex = []
    angles = []
    for match in matches:
        full, x_s, y_s = match
        x = float(x_s)/scale
        y = float(y_s)/scale
        points.append((x, y))
        pos = complex(f"{x}+{y}j")
        points_complex.append(pos)
        theta_match = re.search(r'rotate\((\d+(?:\.\d+)?)[^>]*', full)
        if theta_match:
            theta = float(theta_match.group(1))
        else:
            theta = 0
        angles.append(theta)
    return groups(points, points_complex, angles)

def groups(points, points_complex, angles):
    groups = [(False, [])]
    def add_pos(point, angle):
        groups[-1][1].append((*point, angle))

    reversing = False
    for pt, point, angle, next_point in zip(points, points_complex, angles, points_complex[1:]):
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

def out_format(groups):
    indent = "  "
    s = ""
    for group in groups:
        reversed, positions = group
        m = 1 if reversed else -1
        points = map(lambda x: indent + f"Point{{{x[0]}_in, {m*x[1]}_in, {m*x[2]}_deg}}", positions)
        points_str = ",\n".join(points)
        s += "travelProfile({\n" + points_str + "\n}, " + ("true" if reversed else "false") + ", 0.5);\n"
    return s
