# This universal Makefile is prepared for different platforms. 

# Platform Specific Settings
ifeq ($(OS),Windows_NT)
  ifdef SystemRoot
    ifdef VSINSTALLDIR
      # ----------------- Windows visual c --------------------------------
      CXX = $(VSINSTALLDIR)\VC\bin\cl.exe
      LD  = $(VSINSTALLDIR)\VC\bin\link.exe
      CXXFLAGS  = /nologo /W1 /EHsc
      CXXFLAGS += -DOS_WIN32_NATIVE -DFTGL_LIBRARY -DWIN32 -D_WIN32 -D_WINDOWS -D_MBCS -D_CRT_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_DEPRECATE
      CXXFLAGS += -I$(LIB_SDL_DIR)/include
      CXXFLAGS += -IC:/Projects/freetype-2.3.5-1/include
      CXXFLAGS += -IC:/Projects/freetype-2.3.5-1/include/freetype2
      LDFLAGS  = /MACHINE:X86
      LDFLAGS += OpenGL32.lib GLU32.lib SDL.lib SDL_image.lib SDL_mixer.lib freetype.lib
      LDFLAGS += -LIBPATH:$(LIB_SDL_DIR)/lib/x86
      LDFLAGS += -LIBPATH:C:/Projects/freetype-2.3.5-1/lib
      OBJ_EXT = obj
    endif #VSINSTALLDIR
  else
    CXX ?= g++
    LD = $(CXX)
    OBJ_EXT = o

    # ----------------- Windows with mingw --------------------------------
    CXXFLAGS = -Wall -Wextra -O2 -DOS_WIN32_MINGW -mwindows -I/usr/include -I/usr/include/freetype2
    LDFLAGS  = -L/usr/lib -lSDL -lSDL_image -lSDL_mixer -lfreetype
    LDFLAGS += -lopengl32 -lGLU32

    # ----------------- Windows, erins mingw environment ;-) --------------
    # CXXFLAGS = -Wall -Wextra -O2 -DOS_WIN32_MINGW -Ic:/mingw/include/freetype2
    # LDFLAGS = -Lc:/mingw/lib/sdl -lmingw32 -mwindows -lSDLmain -lSDL -lopengl32 -lglu32 \
    # -l:SDL_image.lib -l:SDL_mixer.lib -lfreetype
  endif #SystemRoot
else
  UNAME := $(shell uname)

  ifeq ($(UNAME), Darwin)
    # ----------------- MAC OS --------------------------------------------
    CXX ?= g++
    LD = $(CXX)
    OBJ_EXT = o

    CXXFLAGS  = -Wall -Wextra -O2 -DOS_MAC -framework SDL -framework SDL_image -framework SDL_mixer -framework Freetype
    CXXFLAGS += -framework OpenGL
    LDFLAGS  = -framework Cocoa -framework SDL -framework SDL_image -framework SDL_mixer -framework Freetype 
    LDFLAGS += -framework OpenGL
    OBJ += SDLMain.$(OBJ_EXT)
  else
    # ----------------- Linux (Default) -----------------------------------
    CXX ?= g++
    LD = $(CXX)
    OBJ_EXT = o

    CXXFLAGS = -Wall -Wextra -O2 -DOS_LINUX -I/usr/include/freetype2
    LDFLAGS  = -lSDL -lSDL_image -lSDL_mixer -lfreetype 

    ifdef PROFILE
      CXXFLAGS += -pg
      LDFLAGS += -pg
    endif #PROFILE

    ifdef GLES
      CXXFLAGS += -DHAVE_GL_GLES1
      LDFLAGS  += -lGLESv1_CM -lEGL
      #Pandaboard
      CXXFLAGS += -I/usr/lib/pvr-omap4-egl/include
      LDFLAGS  += -L/usr/lib/pvr-omap4-egl
      #Raspberry Pi
      CXXFLAGS += -I/opt/vc/include
      CXXFLAGS += -I/opt/vc/include/interface/vcos/pthreads
      LDFLAGS  += -L/opt/vc/lib
    else
      LDFLAGS += -lGL -lGLU
    endif #GLES
  endif #Darwin
endif #Windows_NT

# Targets
BIN = etr
OBJ += main.$(OBJ_EXT) game_config.$(OBJ_EXT) ogl.$(OBJ_EXT) tux.$(OBJ_EXT) audio.$(OBJ_EXT) \
winsys.$(OBJ_EXT) particles.$(OBJ_EXT) mathlib.$(OBJ_EXT) splash_screen.$(OBJ_EXT) intro.$(OBJ_EXT) racing.$(OBJ_EXT) \
game_over.$(OBJ_EXT) paused.$(OBJ_EXT) reset.$(OBJ_EXT) game_type_select.$(OBJ_EXT) event_select.$(OBJ_EXT) \
race_select.$(OBJ_EXT) credits.$(OBJ_EXT) loading.$(OBJ_EXT) course.$(OBJ_EXT) keyframe.$(OBJ_EXT) env.$(OBJ_EXT) \
event.$(OBJ_EXT) spx.$(OBJ_EXT) common.$(OBJ_EXT) course_render.$(OBJ_EXT) game_ctrl.$(OBJ_EXT) physics.$(OBJ_EXT) \
track_marks.$(OBJ_EXT) hud.$(OBJ_EXT) view.$(OBJ_EXT) gui.$(OBJ_EXT) translation.$(OBJ_EXT) tools.$(OBJ_EXT) \
quadtree.$(OBJ_EXT) font.$(OBJ_EXT) ft_font.$(OBJ_EXT) textures.$(OBJ_EXT) help.$(OBJ_EXT) regist.$(OBJ_EXT) \
tool_frame.$(OBJ_EXT) tool_char.$(OBJ_EXT) newplayer.$(OBJ_EXT) score.$(OBJ_EXT) ogl_test.$(OBJ_EXT)

# Rules
$(BIN) : $(OBJ)
	$(LD) -o $(BIN) $(OBJ) $(LDFLAGS)

clean :
	rm -f $(BIN) $(OBJ)

# use this template and rename it if you want to add a module

# mmmm.$(OBJ_EXT) : mmmm.cpp mmmm.h
#	$(CXX) -c mmmm.cpp $(CXXFLAGS)

# MAC OS
SDLMain.$(OBJ_EXT) : SDLMain.m SDLMain.h
	$(CXX) -c SDLMain.m

# General
ogl_test.$(OBJ_EXT) : ogl_test.cpp ogl_test.h
	$(CXX) -c ogl_test.cpp $(CXXFLAGS)

score.$(OBJ_EXT) : score.cpp score.h
	$(CXX) -c score.cpp $(CXXFLAGS)

newplayer.$(OBJ_EXT) : newplayer.cpp newplayer.h
	$(CXX) -c newplayer.cpp $(CXXFLAGS)

tool_char.$(OBJ_EXT) : tool_char.cpp tool_char.h
	$(CXX) -c tool_char.cpp $(CXXFLAGS)

tool_frame.$(OBJ_EXT) : tool_frame.cpp tool_frame.h
	$(CXX) -c tool_frame.cpp $(CXXFLAGS)

regist.$(OBJ_EXT) : regist.cpp regist.h
	$(CXX) -c regist.cpp $(CXXFLAGS)

tools.$(OBJ_EXT) : tools.cpp tools.h
	$(CXX) -c tools.cpp $(CXXFLAGS)

help.$(OBJ_EXT) : help.cpp help.h
	$(CXX) -c help.cpp $(CXXFLAGS)

translation.$(OBJ_EXT) : translation.cpp translation.h
	$(CXX) -c translation.cpp $(CXXFLAGS)

physics.$(OBJ_EXT) : physics.cpp physics.h
	$(CXX) -c physics.cpp $(CXXFLAGS)

winsys.$(OBJ_EXT) : winsys.cpp winsys.h
	$(CXX) -c winsys.cpp $(CXXFLAGS)

game_ctrl.$(OBJ_EXT) : game_ctrl.cpp game_ctrl.h
	$(CXX) -c game_ctrl.cpp $(CXXFLAGS)

textures.$(OBJ_EXT) : textures.cpp textures.h
	$(CXX) -c textures.cpp $(CXXFLAGS)

ft_font.$(OBJ_EXT) : ft_font.cpp ft_font.h
	$(CXX) -c ft_font.cpp $(CXXFLAGS)

font.$(OBJ_EXT) : font.cpp font.h
	$(CXX) -c font.cpp $(CXXFLAGS)

event.$(OBJ_EXT) : event.cpp event.h
	$(CXX) -c event.cpp $(CXXFLAGS)

gui.$(OBJ_EXT) : gui.cpp gui.h
	$(CXX) -c gui.cpp $(CXXFLAGS)

common.$(OBJ_EXT) : common.cpp common.h
	$(CXX) -c common.cpp $(CXXFLAGS)

spx.$(OBJ_EXT) : spx.cpp spx.h
	$(CXX) -c spx.cpp $(CXXFLAGS)

quadtree.$(OBJ_EXT) : quadtree.cpp quadtree.h
	$(CXX) -c quadtree.cpp $(CXXFLAGS)

view.$(OBJ_EXT) : view.cpp view.h
	$(CXX) -c view.cpp $(CXXFLAGS)

hud.$(OBJ_EXT) : hud.cpp hud.h
	$(CXX) -c hud.cpp $(CXXFLAGS)

track_marks.$(OBJ_EXT) : track_marks.cpp track_marks.h
	$(CXX) -c track_marks.cpp $(CXXFLAGS)

course_render.$(OBJ_EXT) : course_render.cpp course_render.h
	$(CXX) -c course_render.cpp $(CXXFLAGS)

env.$(OBJ_EXT) : env.cpp env.h
	$(CXX) -c env.cpp $(CXXFLAGS)

keyframe.$(OBJ_EXT) : keyframe.cpp keyframe.h
	$(CXX) -c keyframe.cpp $(CXXFLAGS)

course.$(OBJ_EXT) : course.cpp course.h
	$(CXX) -c course.cpp $(CXXFLAGS)

loading.$(OBJ_EXT) : loading.cpp loading.h
	$(CXX) -c loading.cpp $(CXXFLAGS)

credits.$(OBJ_EXT) : credits.cpp credits.h
	$(CXX) -c credits.cpp $(CXXFLAGS)

race_select.$(OBJ_EXT) : race_select.cpp race_select.h
	$(CXX) -c race_select.cpp $(CXXFLAGS)

event_select.$(OBJ_EXT) : event_select.cpp event_select.h
	$(CXX) -c event_select.cpp $(CXXFLAGS)

game_type_select.$(OBJ_EXT) : game_type_select.cpp game_type_select.h
	$(CXX) -c game_type_select.cpp $(CXXFLAGS)

game_over.$(OBJ_EXT) : game_over.cpp game_over.h
	$(CXX) -c game_over.cpp $(CXXFLAGS)

paused.$(OBJ_EXT) : paused.cpp paused.h
	$(CXX) -c paused.cpp $(CXXFLAGS)

reset.$(OBJ_EXT) : reset.cpp reset.h
	$(CXX) -c reset.cpp $(CXXFLAGS)

racing.$(OBJ_EXT) : racing.cpp racing.h
	$(CXX) -c racing.cpp $(CXXFLAGS)

intro.$(OBJ_EXT) : intro.cpp intro.h
	$(CXX) -c intro.cpp $(CXXFLAGS)

splash_screen.$(OBJ_EXT) : splash_screen.cpp splash_screen.h
	$(CXX) -c splash_screen.cpp $(CXXFLAGS)

mathlib.$(OBJ_EXT) : mathlib.cpp mathlib.h
	$(CXX) -c mathlib.cpp $(CXXFLAGS)

particles.$(OBJ_EXT) : particles.cpp particles.h
	$(CXX) -c particles.cpp $(CXXFLAGS)

audio.$(OBJ_EXT) : audio.cpp audio.h
	$(CXX) -c audio.cpp $(CXXFLAGS)

tux.$(OBJ_EXT) : tux.cpp tux.h
	$(CXX) -c tux.cpp $(CXXFLAGS)

ogl.$(OBJ_EXT) : ogl.cpp ogl.h
	$(CXX) -c ogl.cpp $(CXXFLAGS)

game_config.$(OBJ_EXT) : game_config.cpp game_config.h
	$(CXX) -c game_config.cpp $(CXXFLAGS)

main.$(OBJ_EXT) : main.cpp bh.h etr_types.h
	$(CXX) -c main.cpp $(CXXFLAGS)
install:
	cp -r data/ /usr/local/share/etr/data/
	cp etr  /usr/local/share/etr
	ln -s /usr/local/share/etr/etr /usr/local/bin/etr
	cp etr.desktop /usr/local/share/applications/
	cp etr.png /usr/local/share/etr/
