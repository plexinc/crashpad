#!/usr/bin/env python3

from pathlib import Path
import re
import os
import shutil

# We don't want all of the deps that upstream needs
deps_we_want = ("gtest", "lss", "mini_chromium")


def is_third_party(line):
  return "crashpad/third_party" in line


def is_matching_dep(line):
  for dep in deps_we_want:
    if f"crashpad/third_party/{dep}" in line:
      return dep
  return None


if __name__ == "__main__":
  root = Path(__file__).parent.resolve()

  deps = {}
  with open(root / "DEPS", "r") as dfp:
    dep_props = None
    all_deps = []

    path_re = re.compile("Var\('chromium_git'\)\s\+\s'([a-zA-Z-_/.]+)")
    sha_re = re.compile("'([a-z0-9]{40})'")
    for line in dfp:
      line = line.strip()

      istp = is_third_party(line)
      if istp:
        if dep_props:
          all_deps.append(dep_props)
          dep_props = None

        dep = is_matching_dep(line)

        if dep:
          dep_props = {"path": line.split("'")[1].replace("crashpad/", ""), "name": dep}

      elif dep_props:
        match = path_re.match(line)
        if match:
          dep_props["git"] = match.group(1).replace(".git", "")
          continue

        match = sha_re.match(line)
        if match:
          dep_props["sha"] = match.group(1)
          continue

    for dep in all_deps:
      from subprocess import run

      if os.path.isdir(dep["path"]):
        shutil.rmtree(dep["path"])
      os.makedirs(dep["path"])

      url = f"https://artifacts.plex.tv/cache-googlesource{dep['git']}/+archive/{dep['sha']}.tar.gz"
      fname = f"{dep['path']}/{dep['name']}.tar.gz"
      print(url)
      run(["curl", "-L", "--fail", "-o", fname, url], check=True)
      run(["tar", "-xzf", f"{dep['name']}.tar.gz"], cwd=dep["path"])
      os.unlink(fname)
