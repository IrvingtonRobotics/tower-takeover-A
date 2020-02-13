#!/usr/bin/env python3

import re
import math
import cmath
from enum import Enum, auto
from functools import reduce

INDENT = "  "
DEFAULT_SPEED = 0.20
DISTANCE_TOL = 0.01

class Tag(Enum):
  POSITION = auto()
  MACRO = auto()
  VERBATIM = auto()

class Position:
  def __init__(self, start, x, y, theta):
    self.type = Tag.POSITION
    self.start = start
    self.x = x
    self.y = y
    self.theta = theta

  def dist(self, other):
    return ((self.x-other.x)**2 + (self.y-other.y)**2)**0.5

  def __str__(self):
    return f"Position({self.x}, {self.y}, {self.theta})"

hasPosInfo = False;
hasAngleInfo = False;

class Macro:
  def __init__(self, start, command, parameters):
    self.type = Tag.MACRO
    self.start = start
    self.command = command
    self.parameters = parameters

  def run(self, positions):
    global hasPosInfo
    global hasAngleInfo
    params = self.parameters

    if self.command == "suck":
      assert len(positions) <= 1, "Should not have a position stack on suck"
      assert len(params) == 0, "Suck speed not implemented"
      yield "intake.move(QUICK_SUCK_SPEED);"

    elif self.command in ["forward", "back"]:
      assert len(positions) > 1, "Should move between several points"
      assert len(params) <= 1, "Too many arguments for movement operation"
      speed = float(params[0]) if len(params) == 1 else DEFAULT_SPEED
      for i in range(len(positions)-1):
        assert positions[i].dist(positions[i+1]) > DISTANCE_TOL, "Should have a meaningful distance between moves"
      reversed = self.command != "forward"
      yield "travelProfile({"
      m = 1 if reversed else -1
      for pos in positions:
        x = f(pos.x) + "_in"
        y = f(m*pos.y) + "_in"
        if hasPosInfo:
          x = "expectedPA.x"
          y = f"{m}*expectedPA.y"
        theta = f(m*pos.theta) + "_deg"
        if hasAngleInfo:
          theta = f"{m}*expectedPA.theta"
        hasPosInfo = hasAngleInfo = False
        yield INDENT + f"Point{{{x}, {y}, {theta}}},"
      yield "}, " + ("true" if reversed else "false") + ", " + f(speed) + ");"

    elif self.command == "turn":
      assert len(positions) == 2, "Need two points to turn between"
      assert len(params) <= 1, "Turn speed not implemented"
      preferLeft = False
      preferRight = False
      if len(params) == 1:
        assert params[0] in ["left", "right"]
        preferLeft = params[0] == "left"
        preferRight = params[0] == "right"
      angle = positions[1].theta - positions[0].theta
      # make angle positive
      angle = angle % 360
      if preferLeft:
        angle -= 360
      elif not preferRight:
        # minimize angle to move
        if angle > 180:
          angle -= 360
      angle = f(angle)
      yield f"drive.turnAngle({angle}_deg);"
      if hasAngleInfo:
        yield f"expectedPA.theta += {angle}_deg;"

    elif self.command == "check":
      assert len(params) == 2
      # "#check pos bottom angle left"
      # or just "#check angle left" if pos is inaccessible
      sides = {
        "south": "SOUTH",
        "bottom": "SOUTH",
        "left": "WEST",
        "west": "WEST",
        "top": "NORTH",
        "north": "NORTH",
        "right": "EAST",
        "east": "EAST"
      }
      # side (param[1]) should refer to the direction the two back sensors are facing
      dir = sides[params[1]]
      if params[0] == "pa":
        hasPosInfo = True
        hasAngleInfo = True
        yield f"expectedPA = getPosAngle({dir});"
      elif params[0] == "a":
        hasAngleInfo = True
        yield f"expectedPA.theta = getAngle({dir});"

    else:
      assert False, f"Tag '{self.command}' not implemented"

  def __str__(self):
    return f"Macro({self.command}, {self.parameters})"

class Verbatim:
  def __init__(self, start, text):
    self.type = Tag.VERBATIM
    self.start = start
    self.text = text

  def __str__(self):
    return f"Verbatim({self.text})"

def generate(body, scale):
  tags = []
  tags += positions(body, scale)
  tags += macros(body)
  tags += verbatims(body)
  tags.sort(key=lambda e: e.start, reverse=True)
  return out_format(tags)

def positions(body, scale):
  matches = re.finditer(r'<circle [^>]*?fill-opacity="0.98"[^>]*/>', body)
  float_re = r'-?\d+(?:\.\d+)?'
  for match in matches:
    full = match.group(0)
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
    yield Position(match.start(), x, y, theta)

def macros(body):
  matches = re.finditer(r'<text id="#([^"]*?)(_\d*)?"', body);
  for match in matches:
    s = match.group(1).split(" ")
    command = s[0]
    params = s[1:]
    yield Macro(match.start(), command, params)

def verbatims(body):
  # either comments or `;` terminated statements or combos, not starting with "#" (macro character)
  matches = re.finditer(r'<text id="((?!#)([^"]*?;)?\s*(// [^"]*?)?)(_\d*)?"', body)
  for match in matches:
    s = match.group(1)
    if s != "":
      yield Verbatim(match.start(), s)

def f(x):
  return str(round(x, 4))

def out_format(tags):
  lines = []
  positions = []
  for tag in tags:
    if tag.type == Tag.POSITION:
      positions.append(tag)
    elif tag.type == Tag.MACRO:
      lines += tag.run(positions)
      # positions consumed, but keep the last (new position)
      if len(positions) > 0:
        positions = [positions[-1]]
    elif tag.type == Tag.VERBATIM:
      lines.append(tag.text)
    else:
      assert False, "Unaccepted tag"
  return "\n".join(lines)
