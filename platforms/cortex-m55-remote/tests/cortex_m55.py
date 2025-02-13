#!/usr/bin/env python3
# Copyright (c) 2023-2024 Qualcomm Innovation Center, Inc. All Rights Reserved.
# SPDX-License-Identifier: BSD-3-Clause

import argparse, pprint
import os
from os import sched_get_priority_max
from pathlib import Path
import sys
from sys import exit, stdout
import pexpect
from pexpect import *
import getpass
import subprocess
from subprocess import Popen, PIPE
import signal


def vp_test():
    test = False

    parser = argparse.ArgumentParser()
    parser.add_argument("-e", "--exe", metavar="", help="Path to vp executable")
    parser.add_argument("-l", "--lua", metavar="", help="Path to luafile")
    args = parser.parse_args()
    if not args.exe:
        print("vp executable file not found")
        return test
    if not args.lua:
        print("luafile is required")
        return test
    vp_path = Path(args.exe)
    lua_path = Path(args.lua)

    # start vp platform
    cmd = ["ip", "tuntap", "add", "qbox0", "mode", "tap"]
    proc = subprocess.Popen(
        cmd,
        shell=True,
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )

    child = pexpect.spawn(vp_path.as_posix(), ["--gs_luafile", args.lua])
    child.logfile = stdout.buffer
    child.expect("Test program is running. Listening for interrupts.")
    child.expect("IRQ 17 happened")
    child.expect("NMI happened")
    child.expect("SysTick happened")

    # make sure to use SIGQUIT to terminate the child as this signal is handled in 
    # include/greensocs/gsutils/uutils.h and it should be called for VP proper cleanup.
    child.kill(signal.SIGQUIT)
    child.wait()

    test = True
    return test


assert(vp_test())
