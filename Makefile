PREFIX = /usr
BOOST_PREFIX = $(PREFIX)/local

#
# Compiler flags
#

CCXX   = g++
CC     = gcc
CXXFLAGS = `pkg-config gtkmm-3.0 --cflags` -Wall -Werror -Wextra -std=gnu++20 -isystem $(BOOST_PREFIX)/include/boost_1_74_0
CFLAGS   = -Wall -Werror -Wextra
GTKLIBS = `pkg-config gtkmm-3.0 --libs`

#
# Project files
#

CPPSRCS = $(wildcard *.cpp)
CSRCS = $(wildcard *.c)
OBJS = $(CPPSRCS:.cpp=.o) $(CSRCS:.c=.o)
EXE = gcalc
DESKTOPNANE = com.github.mgeck64.ccalc-gtk3.GCalc.desktop

#
# Debug build settings
#

DBGDIR = debug
DBGEXE = $(DBGDIR)/$(EXE)
DBGOBJS = $(addprefix $(DBGDIR)/, $(OBJS))
DBGDEPS = $(DBGOBJS:%.o=%.d)
DBGFLAGS = -g -O0 -DDEBUG

#
# Release build settings
#

RELDIR = release
RELEXE = $(RELDIR)/$(EXE)
RELOBJS = $(addprefix $(RELDIR)/, $(OBJS))
RELDEPS = $(RELOBJS:%.o=%.d)
RELFLAGS = -Os -DNDEBUG
DESKTOPDIR = $(PREFIX)/share/applications
# set DestDir to allow for staging installations to temporary directories before
# manually moving them to their actual place

.PHONY: all clean debug release remake dbglink rellink install uninstall

# Default build
all: release

#
# Debug rules
#

DBGLINK = $(CCXX) -o $(DBGEXE) $(DBGOBJS) -lccalc-dbg $(GTKLIBS)

debug: make_dbgdir $(DBGEXE)

dbglink: make_dbgdir $(DBGOBJS) # re-link ccalc library
	$(DBGLINK)

$(DBGEXE): $(DBGOBJS)
	$(DBGLINK)

-include $(DBGDEPS)

$(DBGDIR)/%.o: %.cpp
	$(CCXX) -c $(CXXFLAGS) $(DBGFLAGS) -MMD -o $@ $<

$(DBGDIR)/%.o: %.c
	$(CC) -c $(CFLAGS) $(DBGFLAGS) -MMD -o $@ $<

#
# Release rules
#

RELLINK = $(CCXX) -o $(RELEXE) $(RELOBJS) -lccalc-rel $(GTKLIBS)

release: make_reldir $(RELEXE)

rellink: make_reldir $(RELOBJS) # re-link ccalc library
	$(RELLINK)

$(RELEXE): $(RELOBJS)
	$(RELLINK)

-include $(RELDEPS)

$(RELDIR)/%.o: %.cpp
	$(CCXX) -c $(CXXFLAGS) $(RELFLAGS) -MMD -o $@ $<

$(RELDIR)/%.o: %.c
	$(CC) -c $(CFLAGS) $(RELFLAGS) -MMD -o $@ $<

#
# Install/uninstall rules
#

install: make_reldir $(RELDIR)/$(EXE)
	install -D -m755 $(RELDIR)/$(EXE) $(DESTDIR)$(PREFIX)/bin/$(EXE)
	install -D -m644 $(DESKTOPNANE) $(DESTDIR)$(DESKTOPDIR)/$(DESKTOPNANE)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(EXE)
	rm -f $(DESTDIR)$(DESKTOPDIR)/$(DESKTOPNANE)

#
# Other rules
#

make_dbgdir:
	@mkdir -p $(DBGDIR)

make_reldir:
	@mkdir -p $(RELDIR)

remake: clean all


clean:
	@rm -r -f $(RELDIR) $(DBGDIR)
