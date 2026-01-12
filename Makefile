TARGET := XtremeStreamer
OBJS := main.o
LIBS := -lwut -lwups -lgx2 -lcoreinit -lnsysnet

# Usamos rutas relativas para que GitHub no se pierda
include $(WUT_ROOT)/share/wut_rules

all: $(TARGET).wps

%.wps: %.elf
	wups-tool $< $@
