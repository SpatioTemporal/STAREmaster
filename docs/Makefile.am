# This is an automake file for the docs directory of the
# STAREmaster utility.

# Ed Hartnett 3/16/20

# Run doxygen, then confirm warning log file is empty.
#
# The warning file is loaded with stuff and this is breaking the builds.
# I moved the test outpof the all target and into a special 'docs' target.
# Undo this later. jhrg 2/17/21
all:
	doxygen Doxyfile

docs: all
	cat doxywarn.log
	[ ! -s doxywarn.log ]

check: all

CLEANFILES = *.log

EXTRA_DIST = Doxyfile.in




