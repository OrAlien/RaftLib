PACKAGEDIR = ./packages/

PACKAGEFILES = randombase

PACKAGEOBJS = $(addprefix ./packages/, $(PACKAGEFILES))

PACKAGELIBS = -lgsl
