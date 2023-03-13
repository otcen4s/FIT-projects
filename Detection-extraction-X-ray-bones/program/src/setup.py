#!/usr/bin/python3

"""setup.py: Setup module for creating shared object libraries and C files for precompiling the program to fasten it up."""

__author__ = "Matej Otčenáš"


from distutils.core import setup
from Cython.Build import cythonize

setup(name="Modules_C", ext_modules=cythonize('Modules/*.pyx', compiler_directives={'language_level' : "3"}))
