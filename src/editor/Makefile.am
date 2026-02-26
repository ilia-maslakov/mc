EXTRA_DIST =

if USE_INTERNAL_EDIT
noinst_LTLIBRARIES = libedit.la
else
noinst_LTLIBRARIES =
endif

libedit_la_SOURCES = \
	bookmark.c \
	edit-impl.h \
	edit.c edit.h \
	editcomplete.c editcomplete.h \
	editbuffer.c editbuffer.h \
	editcmd.c \
	editdraw.c \
	fold.c \
	editmacros.c editmacros.h \
	editmenu.c \
	editoptions.c \
	editsearch.c editsearch.h \
	editwidget.c editwidget.h \
	../editor-plugins/etags/etags.c \
	../editor-plugins/etags/etags.h \
	../editor-plugins/spell/spell.c \
	../editor-plugins/spell/spell.h \
	format.c \
	syntax.c

AM_CPPFLAGS = $(GLIB_CFLAGS) -I$(top_srcdir)
