#!/usr/bin/python3

"""run.py: This module starts the entire third metacarpal edge detection and measurement program."""

__author__ = "Matej Otčenáš"

import os
import argparse


ap = argparse.ArgumentParser()
ap.add_argument("-i", "--input", required=False,
    help="path to the input dataset directory(e.g. /my/test/dataset/images)")
ap.add_argument("-n", "--name", required=False,
    help="image name from where you want to start the program")
ap.add_argument("-o", "--output", required=False,
    help="path to output directory, if not specified, the output will be in this program directory")
args = vars(ap.parse_args())

input_dir = args["input"]
start_image = args["name"]
output_dir = args["output"]

arg = 'python -W ignore main.py'
if input_dir is not None:
    arg += ' -i ' + input_dir
if start_image is not None:
    arg += " -n " + start_image
if output_dir is not None:
    arg += " -o " + output_dir

os.chdir('./src')
os.system('python setup.py build_ext --inplace -q')
os.system(arg)
os.chdir('../')
