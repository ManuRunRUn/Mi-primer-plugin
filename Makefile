TARGET := XtremeStreamer
OBJS := main.o
LIBS := -lwut -lwups -lgx2 -lcoreinit -lnsysnet

include $(WUT_ROOT)/share/wut_rules

all: $(TARGET).wps

%.wps: %.elf
	wups-tool $< $@
