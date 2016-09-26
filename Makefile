#############################################################################
# Makefile for building: p4.m
#############################################################################

####### Files

MAPLE	      = maple
MPLSTRIP      = mplstrip/mplstrip

SOURCES       = blowup.tex	\
		gcd.tex		\
		infinity.tex	\
		main.tex	\
		num_coeff.tex	\
		p4gcf.tex	\
		realroot.tex	\
		tools.tex	\
		type.tex	\
		weakfocus.tex	\
		writelog.tex

OBJECTS       = blowup.m	\
		gcd.m		\
		infinity.m	\
		main.m		\
		num_coeff.m	\
		p4gcf.m		\
		realroot.m	\
		tools.m		\
		type.m		\
		weakfocus.m	\
		writelog.m

TARGET        = p4.m

.SUFFIXES:	.tex .m
.tex.m:
		$(MPLSTRIP) -M=$(MAPLE) -head $<

all:		$(TARGET)

$(TARGET):	$(OBJECTS)

blowup.m:	blowup.tex
gcd.m:		gcd.tex
infinity.m:	infinity.tex
main.m:		main.tex	\
		blowup.m	\
		gcd.m		\
		infinity.m	\
		num_coeff.m	\
		p4gcf.m		\
		realroot.m	\
		tools.m		\
		type.m		\
		weakfocus.m	\
		writelog.m
num_coeff.m:	num_coeff.tex
p4gcf.m:	p4gcf.tex
realroot.m:	realroot.tex
tools.m:	tools.tex
type.m:		type.tex
weakfocus.m:	weakfocus.tex
writelog.m:	writelog.tex
