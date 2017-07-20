#!/usr/bin/env python

from distutils.core import setup
from distutils.extension import Extension

setup(name="PredictionPkg",
    ext_modules=[
        Extension("prediction", ["module_prediction.cc"],
        libraries = ["boost_python"],
	extra_compile_args=['-std=c++11'])
    ])
