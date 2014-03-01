## Process this file with automake to produce Makefile.in

## Created by Anjuta


uidir = $(pkgdatadir)/ui
ui_DATA = vocabnotify.ui


AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(VOCABNOTIFY_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g\
	 -lgmodule-2.0\
	 -lswni\
	 -I\
	 include/\
	 -std=gnu99

bin_PROGRAMS = vocabnotify

vocabnotify_SOURCES =  \
	callbacks.c \
	database.c \
	gui.c \
	notifier.c \
	setting_manager.c \
	threads.c \
	vocabmanager.c \
	vocabnotify.c \
	wni.c \
	xfsm-fadeout.c \
	callbacks.h \
	database.h \
	gui.h \
	notifier.h \
	setting_manager.h \
	threads.h \
	vocabmanager.h \
	vocabnotify.h \
	xfsm-fadeout.h \
	wni.h

vocabnotify_LDFLAGS = \
	-Wl,--export-dynamic

vocabnotify_LDADD = $(VOCABNOTIFY_LIBS)


EXTRA_DIST = $(ui_DATA)

# Remove ui directory on uninstall
uninstall-local:
	-rm -r $(uidir)
	-rm -r $(pkgdatadir)

