#!/usr/bin/python
import os, sys
import glob
from subprocess import check_output, call
import subprocess

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

def build_device(workdir):
    print "Building device, workdir: ", workdir
    pwd=os.getcwd()
    os.chdir(workdir)
    call(["cp","thinning.cpp","jni/"])
    out = check_output(["ndk-build"],shell=True)
    print out
    sys.stdout.flush()
    os.chdir(pwd)
    if os.path.exists(os.path.join(workdir,"libs","armeabi-v7a","thinning")):
        print "Build for device: SUCCESS"
        return 0
    else:
        print "Build for device: FAIL"
        return 1

def install_executable(executable):
    call(["adb","push",executable,"/data/local/tmp/"])
    cmd="chmod 777 /data/local/tmp/thinning"
    call(["adb","shell",cmd])

def install_data(datadir,dest):
    call(["adb","shell","mkdir -p",dest])
    call(["adb","push",datadir,dest])

def launch_tests(work_paths,device_data_dir):
    for image in work_paths:
        image=image.replace('\\','/')
        cmd="/data/local/tmp/thinning "+device_data_dir+"/"+image+" 10 0"
        h = subprocess.Popen(" ".join(["adb","shell",cmd]),shell=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
        while True:
            line = h.stdout.readline()
            print line,
            sys.stdout.flush()
            if  line == "": break

def main():
    ndk_build=which("ndk-build")

    if not ndk_build:
        print "No ndk-build found, plz add it to your PATH"
        sys.exit(0)


    adb=which('adb')
    if not adb:
        print "No adb found, plz add it to your PATH"
        sys.exit(0)


    opencv_android_sdk_root=os.environ['OPENCV_ANDROID_HOME']
    if not os.path.exists(opencv_android_sdk_root):
        print "opencv android sdk not found, plz set OPENCV_ANDROID_HOME"
        sys.exit(0)


    workdir=os.path.dirname(os.path.realpath(__file__))
    if build_device(workdir):
        print "Error during ndk build"
        sys.exit(1)
    executable=glob.glob(os.path.join(workdir,"libs","armeabi-v7a","thinning"))[0]

    datadir=os.path.join(os.path.dirname(workdir),"data")
    dev = check_output(["adb","devices"])
    if not "device" in dev.split():
        print "No devices connected"
        sys.exit(0)

    install_executable(executable)
    device_data_dir = "/data/local/tmp/data/"
    install_data(datadir,device_data_dir)

    work_paths=[]
    for root,dirs,files in os.walk(datadir):
        for f in files:
            work_paths.append(os.path.join(root[root.find(datadir)+len(datadir)+1:],f))

    launch_tests(work_paths,device_data_dir)

if __name__ == '__main__':
    main()
