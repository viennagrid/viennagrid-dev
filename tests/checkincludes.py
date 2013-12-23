#
# Simple script which checks all header files in the ViennaGrid source tree for being self-sufficient.
# Known to work with Python 2.7.3. Run from tests/ folder as `python checkincludes.py`
# Requires g++ to be in PATH as well as write permissions to this folder.
#
# License: MIT/X11, see LICENSE file in top level directory
#

import os
import subprocess

#
# Checks all files in 'files' to be self-sufficient by creating a simple source file with contents:
#   #include <file>
#   int main() {}
# and hands it over to the compiler. In the case of an error, the compiler output is printed and an exception thrown.
#
def run_include_check(files, compiler_options, compiler_name = "g++"):
  for filename in files:
    #write simple checker file and compile:
    print "Testing " + filename
    sourcefilename = "test-self-sufficient.cpp"
    file = open(sourcefilename, "w")
    file.write('#include "' + filename + '"' + "\n")
    file.write("int main() { return 0; }")
    file.close()
    try:
      subprocess.check_output(compiler_name + " " + sourcefilename + " " + compiler_options, stderr=subprocess.STDOUT, shell=True)
    except subprocess.CalledProcessError as e:
      print "ERROR: ",e.output
      raise

#
# Iterates through
#
def run_check(root_path, exclude_dirs, exclude_files, compiler_options, compiler_name = "g++"):
  files_to_check=[]
  for root, dirs, files in os.walk(root_path):
    for filename in files:
      if not root in exclude_dirs:
        if not filename in exclude_files:
         files_to_check.append(os.path.join(root, filename))

  run_include_check(files_to_check, compiler_options, compiler_name)

###

rootPath = '../viennagrid'

print " --- Running self sufficiency tests --- "

exclude_files=[
"seed_point_segmenting.hpp",
"serialization.hpp"
]

exclude_dirs=[]
  
run_check(rootPath, exclude_dirs, exclude_files, "-I..")


