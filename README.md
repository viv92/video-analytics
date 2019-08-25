Steps to build the aiSaac_lite repository including the prerequisites:

-------------------------------------------------

1. Install opencv
    + Clone OpenCV version 3.3 from the OpenCV source code website
    + Install all dependencies given on the build from source page
    + Install libx264 and ffmpeg
    + Make OpenCV by following the steps given on the website

2. Build src/utils/datk
    + Create build folder inside ```datk/```
    + From inside build folder, run ```cmake ..```
    + ```make all ```

3. Navigate to ```tools/``` and modify ```developerConfig.h``` to suit your local environment - copy it from somewhere else if you do not have

4. Build ```src/utils/ccv```
    + cd src/utils/ccv/lib
    + Copy ```Makefile.example``` to ```Makefile```
    + ```make libccv.a```

5. Install vlc
    + ```sudo apt-get install libvlc-dev```
    + ```sudo apt-get install vlc```

6. Install Boost
	+ ```sudo apt-get install libboost-all-dev```

7. Install Glog
	+ ```sudo apt-get install libgoogle-glog-dev```

8. Install FFTW3
  + ```sudo apt-get install libfftw3-dev```

9. If building on a raspberry-Pi: Build src/utils/raspicam-0.1.6
	+ Create build folder inside ```raspicam-0.1.6/```
	+ From inside build folder, run ```cmake ..```
	+ ```make```
	+ ```sudo make install```
	+ ```sudo ldconfig -v```

10. Build aisaac_lite
    + Copy Makefile.example to Makefile
    + Create a build folder inside aisaac_lite/
    + Edit the aisaac_lite Makefile to change path of all the libraries appropriately (for both libs and includes)
    + ```make <recipe_name>```

11. Get a copy of aiSaacSettingsConfig.json and encoderCameraSettingsConfig.json and store it in aisaac_lite/build/storageDir

12. Seed the storage directory with pre trained models, haar cascades etc. from <sth>.rar

13. Create appropriate folders in storage directory
