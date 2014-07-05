Extreme Tux Racer
=================

This is a fork of extremetuxracer (http://extremetuxracer.com), focussing on cross-platform, performance & OpenGL-ES support.

**Aims**

As development of extremetuxracer appears to have stopped, I forked the main development trunk at revision r255, with the main intentions of :

* Better performance for linux/arm targets. My main interest is the Beagleboard/Pandaboard and RaspberryPi which only have 3D hardware acceleration under OpenGLES. I've updated the OpenGL calls to support both GL/GLES1 - the GLES code isn't linux/arm specific so can be used for any GLES targets.

* Fixing the OSX build. Most notably the texture formats didn't appear to correctly deal with the possible SDL image loader formats (GL_RGB vs GL_BGR).

* Cross platform Makefile. The Makefile required the user to uncomment the required target, I've updated this so that the Makefile tests for environment variables to determine whether the target is Darwin(OSX), Windows MinGW, Windows Visual Studio or Linux (general). 

**Building**

For the default (OpenGL) build type:

    make

For the OpenGLES build type:

    make GLES=1

To enable gprof profiling type:

    make PROFILE=1

or

    make GLES=1 PROFILE=1

It is safe to build in parallel using the '-j' option e.g

    make GLES=1 -j4
