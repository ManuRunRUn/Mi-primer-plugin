TARGET := XtremeStreamer
OBJS := main.o
LIBS := -lwut -lwups -lgx2 -lcoreinit -lnsysnet

include $(DEVKITPPC)/base_tools
include $(WUT_ROOT)/share/wut_rules

CFLAGS += -O3 -Wall
CXXFLAGS += -O3 -Wall

all: $(TARGET).wps

%.wps: %.elf
	wups-tool $< $@
