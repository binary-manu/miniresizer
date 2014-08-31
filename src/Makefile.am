bin_PROGRAMS = miniresizer
miniresizer_SOURCES = Fl_Value_Output_3Digit.hpp miniresizer_core.cpp \
    miniresizer_core.hpp miniresizer.fld
nodist_miniresizer_SOURCES = miniresizer.cpp miniresizer.hpp
BUILT_SOURCES =  miniresizer.cpp miniresizer.hpp
EXTRA_miniresizer_SOURCES = res/win/winres.rc
EXTRA_DIST = res
CLEANFILES = miniresizer.cpp miniresizer.hpp
AM_CPPFLAGS = -D__STDC_CONSTANT_MACROS
AM_LDFLAGS =
vpath %.rc $(srcdir)/res/win/

%.cpp %.hpp : %.fld
	$(FLUID) -c $<

if WIN32
  ## Use the DLL version of FLTK
  AM_CPPFLAGS += -DFL_DLL
  AM_LDFLAGS += -mwindows
  miniresizer_LDADD = winres.$(OBJEXT)
  
  ## Add a rule for Windows resources compilation
  %.$(OBJEXT) : %.rc
	$(WINDRES) -o $@ -i $<
else
  ##Install XDG stuff

  icon16dir = $(prefix)/share/icons/hicolor/16x16/apps
  icon24dir = $(prefix)/share/icons/hicolor/24x24/apps
  icon32dir = $(prefix)/share/icons/hicolor/32x32/apps
  icon48dir = $(prefix)/share/icons/hicolor/48x48/apps
  icon64dir = $(prefix)/share/icons/hicolor/64x64/apps
  icon72dir = $(prefix)/share/icons/hicolor/72x72/apps
  icon96dir = $(prefix)/share/icons/hicolor/96x96/apps
  icon128dir = $(prefix)/share/icons/hicolor/128x128/apps
  desktopdir = $(prefix)/share/applications

  icon16_DATA = res/icons/16x16/miniresizer.png
  icon24_DATA = res/icons/24x24/miniresizer.png
  icon32_DATA = res/icons/32x32/miniresizer.png
  icon48_DATA = res/icons/48x48/miniresizer.png
  icon64_DATA = res/icons/64x64/miniresizer.png
  icon72_DATA = res/icons/72x72/miniresizer.png
  icon96_DATA = res/icons/96x96/miniresizer.png
  icon128_DATA = res/icons/128x128/miniresizer.png
  desktop_DATA = res/desktop/miniresizer.desktop
endif

