ifndef NAVISERVER
    NAVISERVER  = /usr/local/ns
endif

#
# Module name
#
MOD      =  libtqrcodegen.so

#
# Objects to build.
#
MODOBJS     = library.o qrcodegen.o

MODLIBS  +=

include  $(NAVISERVER)/include/Makefile.module