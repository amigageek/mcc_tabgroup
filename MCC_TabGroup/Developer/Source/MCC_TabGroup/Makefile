CC		= vc
CFLAGS		=					\
		+aos68k					\
		-c99					\
		-warn=-1				\
		-dontwarn=51,81,163,166,167,170,306,307	\
		-O4					\
		-cpu=68000				\
		-I../../C/Include			\
		-lamiga					\
		-lvc					\
		-nostdlib

MCC		= ../../../Libs/MUI/TabGroup.mcc
MCP		= ../../../Libs/MUI/TabGroup.mcp
AUTODOC		= ../../Autodocs/MCC_TabGroup.doc
PREFSIMAGE	= PrefsImage.h
MCC_SRCS	= Common.c Tab.c TabGroup.c
MCP_SRCS	= Prefs.c
PREFSIMAGE_SRCS	= PrefsImage.iff
AUTODOC_SRCS	= Autodoc.h

all: $(MCC) $(MCP)

clean:
	rm $(MCC) $(MCP)

.PHONY: $(MCC)
$(MCC):
	$(CC) -o $@ $(MCC_SRCS) $(CFLAGS)

.PHONY: $(MCP)
$(MCP):
	$(CC) -o $@ $(MCP_SRCS) $(CFLAGS)

.PHONY: $(AUTODOC)
$(AUTODOC): $(AUTODOC_SRCS)
	vamos Autodoc -- $< >$@

.PHONY: $(PREFSIMAGE)
$(PREFSIMAGE): $(PREFSIMAGE_SRCS)
	vamos brush2c $< | head -n -1 >$@
