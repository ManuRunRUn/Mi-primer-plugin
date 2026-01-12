# Nombre del archivo de salida (.wps)
TARGET := XtremeStreamer

# Directorios de herramientas
WUT_ROOT := $(DEVKITPRO)/wut
WUPS_ROOT := $(DEVKITPRO)/wups

# Reglas de WUPS (Aroma)
include $(WUPS_ROOT)/wups_rules

# Librerías necesarias para que funcione la red y el video
LIBS := -lwups -lgx2 -lcoreinit -lnetwork -lnsysnet

# Optimización para máxima fluidez
CFLAGS += -O3 -Wall
CXXFLAGS += -O3 -Wall

# Reglas finales de compilación
include $(DEVKITPRO)/devkitPPC/base_rules
