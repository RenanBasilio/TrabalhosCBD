AUTOMAKE_OPTIONS = foreign subdir-objects
ACLOCAL_AMFLAGS = -I m4
SUBDIRS = lib/vhdf-1.0.0

AM_CXXFLAGS = -std=c++11 -Wl,-rpath='$(top_builddir)/lib/vhdf-1.0.0/.libs'
AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/lib/vhdf-1.0.0/include
EXTRA_DIST = \
  CMakeLists.txt \
  README.md

bin_PROGRAMS = Lista_02
Lista_02_SOURCES = src/main.cpp src/Util.cpp src/Parser.cpp src/Registro.cpp src/Data.cpp src/BlockIO.cpp
Lista_02_LDADD = $(top_builddir)/lib/vhdf-1.0.0/.libs/libvhdf.so