opt_course
==========

Optimization course home exersize

To run cpu tests:
./run_cpu.py

It will build the executable and perform tests on the images from data/VGA folder. The executable is built with scons,
hence it is to be installed on target machine. The compiler is hardcoded to "icl" in the Sconstruct file. So, plz set
env['CXX']='icl' to the your compiler name. Also plz specify your OpenCV path in the Sconstruct file.


=================

To run device tests:
./run_device.py

Firstly, one need to set OPENCV_ANDROID_HOME pointing to the OpenCV Android SDK. Secondly, ndk-build has to be in PATH.
The run_device.py script will build executable for android device, move the test data images to /data/local/tmp/data, move
executable in /data/local/tmp and launch tests.
