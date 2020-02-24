# makefile for ffjpeg project
# written by rockcarry

SUBDIRS = src

all : $(SUBDIRS)

$(SUBDIRS) : ECHO
	$(MAKE) -C $@

ECHO :
	@echo $(SUBDIRS)

clean :
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done
