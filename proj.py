#!/usr/bin/env python3

import argparse
import subprocess
import json
import os
from colorama import Fore, Style

opts = {
  "small": {
    "name": "Small Auton (44730A)",
    "slot": 1,
    "files": {
      "include/config.hpp": "#define MODE SMALL_SIDE"
    }
  },
  "big": {
    "name": "Big Auton (44730A)",
    "slot": 2,
    "files": {
      "include/config.hpp": "#define MODE BIG_SIDE"
    }
  },
  "skills": {
    "name": "Auton Skills (44730A)",
    "slot": 3,
    "files": {
      "include/config.hpp": "#define MODE SKILLS"
    }
  },
  "driver": {
    "name": "Driver Skills (44730A)",
    "slot": 4,
    "files": {
      "include/config.hpp": "#define MODE DRIVER"
    }
  },
}

pp_start = "project.pros"
pp_bak = ".project.pros.bak"

def brightprint(*args, **kwargs):
  print(Fore.CYAN)
  print(*args, **kwargs)
  print(Style.RESET_ALL)

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
    project["py/state"]["project_name"] = opt["name"]
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
    brightprint("Press enter when you are ready to upload or anything else to cancel:", end='')
    response = input()
    if response != "":
      # response is not empty (just hit enter)
      raise Exception("User said no")
  brightprint("Uploading...")
  p2 = subprocess.run(["prosv5", "upload", "--slot", str(opt["slot"])])
  if p2.returncode != 0:
    # upload failed
    raise ChildProcessError()
  revert()
  if terminal:
    brightprint("Opening terminal...")
    subprocess.run(["prosv5", "terminal"])

def run_safe(make_all, upload_immediate, terminal, mode):
  try:
    run(make_all, upload_immediate, terminal, mode)
  except:
    # this is probably a PROS error, such as brain not connected, which has its own error message
    # otherwise this is almost surely the user saying "no", which they don't need feedback on
    pass
  finally:
    revert()


# ./proj.py -m small
parser = argparse.ArgumentParser(description="Autogenerate motion profile from SVG.")
parser.add_argument("mode", choices=opts.keys(),
  help="Mode: any of " + str(list(opts.keys())))
group = parser.add_mutually_exclusive_group()
group.add_argument("--quick", "-q", action="store_true", help="Make quick")
group.add_argument("--all", "-a", action="store_true", help="Make all, not just quick")
parser.add_argument("--upload-immediate", "-u", action="store_true", help="Upload without a confirmation")
parser.add_argument("--terminal", "-t", action="store_true", help="Open terminal after uploading")

args = parser.parse_args()
all = args.all and not args.quick
run_safe(all, args.upload_immediate, args.terminal, args.mode)
