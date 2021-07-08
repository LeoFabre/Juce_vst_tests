# JUCE VST Tests
This repo contains some tests and bootstraps that I went through while learning the amazing JUCE framework.
It is based on Eyal Amir's super cool JUCE CMake prototype repo. 

To build, all you have to do is load this project in your favorite CMake compatible IDE 
and click 'build' for one of the targets.

You can also generate a project for an IDE by using:
```cmake
cmake -G Xcode -B build 
```

For package management, I'm using the amazing CPM.cmake:
#https://github.com/TheLartians/CPM.cmake
It automatically fetches JUCE from git, but you can also set the variable:
CPM_JUCE_SOURCE to point it to a local folder, by using:
``-DCPM_JUCE_SOURCE="Path_To_JUCE"``
when invoking CMake

Have fun !
Leo
Original readme by Eyal Amir
