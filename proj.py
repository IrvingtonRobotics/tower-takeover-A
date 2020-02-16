#!/usr/bin/env python3

import argparse
import subprocess
import json
import os
from colorama import Fore, Style, init
import re
from datetime import datetime

init()

# 8 slot limit
opts = {
  "smallblue": {
    "name": "Small Blue",
    "slot": 1,
    "files": {
      "include/config.hpp": "#define MODE SMALL_SIDE\n#define IS_RED false"
    }
  },
  "smallred": {
    "name": "Small Red",
    "slot": 2,
    "files": {
      "include/config.hpp": "#define MODE SMALL_SIDE\n#define IS_RED true"
    }
  },
  "skills": {
    "name": "Auton Skills",
    "slot": 3,
    "files": {
      "include/config.hpp": "#define MODE SKILLS\n#define IS_RED false"
    }
  },
  "driver": {
    "name": "Driver Skills",
    "slot": 4,
    "files": {
      "include/config.hpp": "#define MODE DRIVER\n#define IS_RED false"
    }
  },
  "bigblue": {
    "name": "Big Blue",
    "slot": 5,
    "files": {
      "include/config.hpp": "#define MODE BIG_SIDE\n#define IS_RED false"
    }
  },
  "bigred": {
    "name": "Big Red",
    "slot": 6,
    "files": {
      "include/config.hpp": "#define MODE BIG_SIDE\n#define IS_RED true"
    }
  },
}

pp_start = "project.pros"
pp_bak = ".project.pros.bak"

def colorprint(color, *args, **kwargs):
  print(color, end='')
  print(*args, **kwargs)
  print(Fore.WHITE)

def brightprint(*args, **kwargs):
  colorprint(Fore.CYAN, *args, **kwargs)

already_reverted = True
def revert():
  # smell
  global already_reverted
  if not already_reverted:
    os.rename(pp_bak, pp_start)
  already_reverted = True

def run(make_all, upload_immediate, terminal, mode):
  global already_reverted
  already_reverted = False
  with open(pp_start, "r") as f:
    project = json.load(f)
  os.rename(pp_start, pp_bak)
  opt = opts[mode]
  with open(pp_start, "w+") as f:
    name = datetime.now().strftime("%H:%M") + " " + opt["name"] + " (44730A)"
    project["py/state"]["project_name"] = name
    json.dump(project, f)
  brightprint("Making...")
  # change config files
  for path in opt["files"]:
    with open(path, "w+") as f:
      f.write(opt["files"][path])
      f.write("\n")
  c1 = ["prosv5", "make"]
  # always make
  if make_all:
    c1.append("all")
  p1 = subprocess.run(c1)
  if p1.returncode != 0:
    # make failed
    raise ChildProcessError()
  brightprint("Done making.")
  # maybe: wait to upload
  if not upload_immediate:
    brightprint("Press enter when you are ready to upload: ", end='')
    response = input()
  brightprint("Uploading...")
  p2 = subprocess.run(["prosv5", "upload", "--slot", str(opt["slot"])])
  if p2.returncode != 0:
    # upload failed
    raise ChildProcessError()
  revert()
  if terminal:
    brightprint("Opening terminal...")
    subprocess.run(["prosv5", "terminal"])

def run_safe(make_all, upload_immediate, terminal, mode, debug):
  try:
    run(make_all, upload_immediate, terminal, mode)
  except:
    # this is probably a PROS error, such as brain not connected, which has its own error message
    if debug:
      raise
  finally:
    revert()

def check_ports():
  with open("src/ports.hpp", "r") as f:
    lines = f.readlines()
  expected_wires = []
  expected_motors = []
  for line in lines:
    e = re.search(r"#define \S* (\d+|'[A-H]')", line)
    if e:
      ass = e.group(1)
      if ass[0] == "'":
        expected_wires.append(f"ADI {ass[1:-1]}")
      else:
        expected_motors.append(ass)

  with open("README.md", "r") as f:
    lines = f.readlines()
    status = None
    header_rows_left = 0
    for i, line in enumerate(lines):
      if "# Wiring" in line:
        status = expected_wires
        header_rows_left = 2
      elif "# Motors" in line:
        status = expected_motors
        header_rows_left = 2
      else:
        if header_rows_left > 0:
          header_rows_left -= 1
          continue
        e = re.match(r'\| ([^|]+) \|', line)
        if e and status:
          port = e.group(1)
          exp_port = "`" + status.pop(0) + "`";
          if port != exp_port:
            colorprint(Fore.YELLOW, "Warning: ", end='')
            print(f"mismatched ports in README:{i+1} based on ports.hpp")
            print(f"  Got port `{port}` but expected `{exp_port}`")
            break

# ./proj.py -m small
parser = argparse.ArgumentParser(description="Autogenerate motion profile from SVG.")
parser.add_argument("mode", choices=opts.keys(),
  help="Mode: any of " + str(list(opts.keys())))
group = parser.add_mutually_exclusive_group()
group.add_argument("--quick", "-q", action="store_true", help="Make quick")
group.add_argument("--all", "-a", action="store_true", help="Make all, not just quick")
parser.add_argument("--upload-immediate", "-u", action="store_true", help="Upload without a confirmation")
parser.add_argument("--terminal", "-t", action="store_true", help="Open terminal after uploading")
parser.add_argument("--no-ports", "-p", action="store_true", help="Disable checking consistent README ports")
parser.add_argument("--debug", "-d", action="store_true", help="Debug: unsuppress warnings")

args = parser.parse_args()
if not args.no_ports:
  check_ports()
all = args.all or not args.quick
run_safe(all, args.upload_immediate, args.terminal, args.mode, args.debug)
