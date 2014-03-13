#!/usr/bin/python
import os, sys
import glob
from subprocess import call
def which(file):
    for path in os.environ["PATH"].split(":"):
        f = glob.glob(os.path.join(path,file)+"*")
        if len(f) > 0:
            return f[0]
    for path in os.environ["PATH"].split(";"):
        f = glob.glob(os.path.join(path,file)+"*")
        if len(f) > 0:
            return f[0]
    return None

def build_cpu(workdir,scons):
    print "Building cpu, workdir: ", workdir
    pwd=os.getcwd()
    os.chdir(workdir)
    call(["python",scons])
    os.chdir(pwd)

def main():
    scons=which("scons")
    workdir=os.path.dirname(os.path.realpath(__file__))
    if not glob.glob(os.path.join(workdir,"thinning_cpu")+"*"):
         build_cpu(workdir,scons)


if __name__ == '__main__':
    main()
