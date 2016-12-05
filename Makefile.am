ACLOCAL_AMFLAGS = -I build-aux

SUBDIRS = src-gui

p4dir=$(prefix)/@PACKAGE@
p4bindir=$(p4dir)/bin
p4helpdir=$(p4dir)/help
p4sumtbdir=$(p4dir)/sum_tables

dist_doc_DATA = README.md
EXTRA_DIST = LICENSE COPYING.LESSER src-mpl help

install-data-local:
	$(MKDIR_P) $(p4sumtbdir)
	cp -r $(srcdir)/help $(p4dir)
	cp $(srcdir)/src-mpl/p4.m $(p4bindir)
	cp $(srcdir)/src-mpl/p4gcf.m $(p4bindir)

install-exec-hook:
	$(MKDIR_P) $(bindir)
	ln -s $(p4bindir)/p4 $(bindir)/p4

install-data-hook:
	chmod 777 $(p4sumtbdir)
	ln $(p4helpdir)/p4_flag.png $(p4bindir)/portrait.png
	ln $(p4helpdir)/newp4icon.png $(p4bindir)/p4smallicon.png
	ln -s $(p4sumtbdir) $(p4dir)/sumtables

uninstall-hook:
	rm -f $(p4dir)/sumtables
	if test -d $(p4bindir); then rm -rf $(p4bindir); fi
	if test -d $(p4helpdir); then rm -rf $(p4helpdir); fi
	if test -d $(p4sumtbdir); then rm -rf $(p4sumtbdir); fi
	if test -d $(p4dir); then rmdir $(p4dir); fi
	if ! test -e $(bindir)/p4; then rm -f $(bindir)/p4; fi
