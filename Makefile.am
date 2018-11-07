AUTOMAKE_OPTIONS = foreign subdir-objects
ACLOCAL_AMFLAGS = -I m4
SUBDIRS = lib/vhdf-1.0.0

AM_CXXFLAGS = -std=c++11 -Wl,-rpath='$$ORIGIN',-rpath='$$ORIGIN/lib/vhdf-1.0.0/.libs'
AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/lib/vhdf-1.0.0/include
EXTRA_DIST = \
  CMakeLists.txt \
  README.md

bin_PROGRAMS = Lista_02
Lista_02_SOURCES = src/main.cpp \
  src/Data.cpp \
  src/DataBlock.cpp \
  src/Parser.cpp \
  src/Query.cpp \
  src/Registro.cpp \
  src/Util.cpp \
  src/MemoryWrapper.cpp \
  src/Campo.cpp \
  src/Hash/Insert.cpp \
  src/Hash/Select.cpp \
  src/Hash/Delete.cpp \
  src/Hash/Test.cpp \
  src/Heap/Insert.cpp \
  src/Heap/Select.cpp \
  src/Heap/Delete.cpp \
  src/Heap/Test.cpp \
  src/Ordered/Insert.cpp \
  src/Ordered/Select.cpp \
  src/Ordered/Delete.cpp \
  src/Ordered/Test.cpp
Lista_02_LDADD = $(top_builddir)/lib/vhdf-1.0.0/.libs/libvhdf.so