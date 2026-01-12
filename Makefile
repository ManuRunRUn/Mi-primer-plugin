# Nombre del plugin
TARGET := XtremeStreamer
OBJS := main.o
LIBS := -lwups -lgx2 -lcoreinit

# Carpetas de herramientas oficiales
include $(DEVKITPRO)/wut/share/wut.mk
include $(DEVKITPRO)/wups/share/wups.mk

# Ajustes de compilación
CFLAGS += -O3 -Wall
CXXFLAGS += -O3 -Wall

all: $(TARGET).wps

# Regla para crear el archivo final de Wii U
%.wps: %.elf
	$(WUPS_TOOL) $< $@
