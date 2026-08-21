TARGET = rpg_fase1
OBJS = main.o

INCDIR =
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS =
LIBS =

BUILD_PRX = 1
PSP_FW_VERSION = 500

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = RPG Fase 1

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
