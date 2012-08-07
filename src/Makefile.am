SUBDIRS = haru ncl

INCLUDES = \
        $(WX_CXXFLAGS)

AM_CXXFLAGS =\
         -Wall\
         -g\
         -DNOJPEG -DNOZLIB -DNOPNG


bin_PROGRAMS = et

et_SOURCES = \
 EpsDC.cpp etDC.cpp main.cc MainWindow.cc PaperOptionsDialog.cpp PdfDC.cpp \
 printdc.cpp TreeFileReader.cpp treelib.c Treeview.cpp wxetdc.cpp \
 EpsDC.h etDC.h MainWindow.h PaperOptionsDialog.h PdfDC.h printdc.h  \
 TreeFileReader.h treelib.h Treeview.h wxetdc.h WmfDC.h WmfDC.cpp \
 DnDFile.h DnDFile.cpp AboutDialog.cc AboutDialog.h eticon.xpm SvgDC.cpp

et_LDFLAGS =

et_LDADD =  \
        $(WX_LIBS) haru/libharu.a ncl/libncl.a 

