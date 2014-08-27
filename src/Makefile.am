bin_PROGRAMS = miniresizer
miniresizer_SOURCES = Fl_Value_Output_3Digit.hpp miniresizer_core.cpp \
    miniresizer_core.hpp miniresizer.fld
nodist_miniresizer_SOURCES = miniresizer.cpp miniresizer.hpp
BUILT_SOURCES =  miniresizer.cpp miniresizer.hpp
EXTRA_miniresizer_SOURCES = winres.rc
EXTRA_DIST = resize.ico
CLEANFILES = miniresizer.cpp miniresizer.hpp
AM_CPPFLAGS = -D__STDC_CONSTANT_MACROS
AM_LDFLAGS =

%.cpp %.hpp : %.fld
	$(FLUID) -c $<

if WIN32
  ## Use the DLL version of FLTK
  AM_CPPFLAGS += -DFL_DLL
  AM_LDFLAGS += -mwindows
  miniresizer_LDADD = winres.$(OBJEXT)
  
  ## Add a rule for Windows resources compilation
  %.$(OBJEXT) : %.rc
	$(WINDRES) $(OUTPUT_OPTION) -i $<
endif

