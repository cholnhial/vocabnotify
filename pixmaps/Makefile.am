SUBDIRS =
EXTRA_DIST = \
	vocabnotify.png

vocabnotifydistpixdir = $(datadir)/pixmaps
vocabnotifydistpix_DATA = $(EXTRA_DIST)

gtk_update_icon_cache = gtk-update-icon-cache -f -t $(datadir)/icons/hicolor

install-data-hook:
	@-if test -z "$(DESTDIR)"; then \
	        echo "Updating Gtk icon cache."; \
	        $(gtk_update_icon_cache); \
	else \
	        echo "*** Icon cache not updated.  After install, run this:"; \
	        echo "***   $(gtk_update_icon_cache)"; \
	fi
