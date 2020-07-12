# This is an automake file for the docs directory of the
# STAREmaster utility.

# Ed Hartnett 3/16/20

# Run doxygen, then confirm warning log file is empty.
all:
	doxygen Doxyfile
	cat doxywarn.log
	[ ! -s doxywarn.log ]

check: all

CLEANFILES = *.log

EXTRA_DIST = Doxyfile.in




