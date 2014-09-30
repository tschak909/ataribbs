ifneq ($(shell echo),)
  CMD_EXE = 1
endif

CBMS = c128   \
       c16    \
       c64    \
       cbm510 \
       cbm610 \
       pet    \
       plus4  \
       vic20

GEOS = geos-apple \
       geos-cbm

TARGETS = atari

#TARGETS = apple2    \
#          apple2enh \
#          atari     \
#          atarixl   \
#          atari5200 \
#          atmos     \
#          $(CBMS)   \
#          $(GEOS)   \
#          lynx      \
#          nes       \
#          sim6502   \
#          sim65c02  \
#          supervision

OUTPUTDIRS := lib         \
              asminc      \
              cfg         \
              include     \
              $(subst ../,,$(filter-out $(wildcard ../include/*.*),$(wildcard ../include/*)))

.PHONY: all mostlyclean clean install zip lib $(TARGETS)

.SUFFIXES:

ifdef CMD_EXE
  DIRLIST = $(strip $(foreach dir,$1,$(wildcard $(dir))))
  MKDIR = mkdir $(subst /,\,$1)
  RMDIR = $(if $(DIRLIST),rmdir /s /q $(subst /,\,$(DIRLIST)))
else
  MKDIR = mkdir -p $1
  RMDIR = $(RM) -r $1
endif

# Every target requires its individual vpath setting but the vpath directive
# acts globally. Therefore each target is built in a separate make instance.

ifeq ($(words $(MAKECMDGOALS)),1)
  ifeq ($(MAKECMDGOALS),$(filter $(MAKECMDGOALS),$(TARGETS)))
    TARGET = $(MAKECMDGOALS)
  endif
endif

ifndef TARGET

datadir = $(prefix)/share/cc65

all lib: $(TARGETS)

mostlyclean:
	$(call RMDIR,../libwrk)

clean:
	$(call RMDIR,../libwrk ../lib) 

ifdef CMD_EXE

install:

else # CMD_EXE

INSTALL = install

define INSTALL_recipe

$(if $(prefix),,$(error variable `prefix' must be set))
$(INSTALL) -d $(DESTDIR)$(datadir)/$(dir)
$(INSTALL) -m644 ../$(dir)/*.* $(DESTDIR)$(datadir)/$(dir)

endef # INSTALL_recipe

install:
	$(foreach dir,$(OUTPUTDIRS),$(INSTALL_recipe))

endif # CMD_EXE

define ZIP_recipe

@cd .. && zip cc65 $(dir)/*.*

endef # ZIP_recipe

zip:
	$(foreach dir,$(OUTPUTDIRS),$(ZIP_recipe))

$(TARGETS):
	@$(MAKE) --no-print-directory $@

else # TARGET

CA65FLAGS =
CC65FLAGS = -Or -W error

## Here in case we need to do per-makefile includes!
#MKINC = $(GEOS) \
#        atari   \
#        atarixl \
#        nes

ifeq ($(TARGET),apple2enh)
  SRCDIR = apple2
  OBJPFX = a2
  DRVPFX = a2e
else ifeq ($(TARGET),atarixl)
  SRCDIR = atari
  OBJPFX = atr
  DRVPFX = atrx
else ifeq ($(TARGET),sim65c02)
  SRCDIR = sim6502
else
  SRCDIR = $(TARGET)
endif

SRCDIRS = $(SRCDIR)

ifeq ($(TARGET),$(filter $(TARGET),$(CBMS)))
  SRCDIRS += cbm
endif

## 
## Additional Source Directories
##
SRCDIRS += common  

vpath %.s $(SRCDIRS)
vpath %.c $(SRCDIRS)

OBJS := $(patsubst %.s,%.o,$(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.s)))
OBJS += $(patsubst %.c,%.o,$(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.c)))

OBJS := $(addprefix ../libwrk/$(TARGET)/,$(sort $(notdir $(OBJS))))

DEPS = $(OBJS:.o=.d)

ifeq ($(TARGET),$(filter $(TARGET),$(MKINC)))
  include $(SRCDIR)/Makefile.inc
endif

AR65 := $(if $(wildcard ../bin/ar65*),../bin/ar65,ar65)
CA65 := $(if $(wildcard ../bin/ca65*),../bin/ca65,ca65)
CC65 := $(if $(wildcard ../bin/cc65*),../bin/cc65,cc65)
LD65 := $(if $(wildcard ../bin/ld65*),../bin/ld65,ld65)

export CC65_HOME := $(abspath ..)

define ASSEMBLE_recipe

$(if $(QUIET),,@echo $(TARGET) - $<)
@$(CA65) -t $(TARGET) $(CA65FLAGS) --create-dep $(@:.o=.d) -o $@ $<

endef # ASSEMBLE_recipe

define COMPILE_recipe

$(if $(QUIET),,@echo $(TARGET) - $<)
@$(CC65) -t $(TARGET) $(CC65FLAGS) --create-dep $(@:.o=.d) --dep-target $@ -o $(@:.o=.s) $<
@$(CA65) -t $(TARGET) -o $@ $(@:.o=.s)

endef # COMPILE_recipe

../libwrk/$(TARGET)/%.o: %.s | ../libwrk/$(TARGET)
	$(ASSEMBLE_recipe)

../libwrk/$(TARGET)/%.o: %.c | ../libwrk/$(TARGET)
	$(COMPILE_recipe)

../lib/modemlib-$(TARGET).lib: $(OBJS) | ../lib
	$(AR65) a $@ $?

../libwrk/$(TARGET) ../lib ../lib/atari:
	@$(call MKDIR,$@)

$(TARGET): $(EXTRA_OBJS) ../lib/modemlib-$(TARGET).lib

-include $(DEPS)

endif # TARGET
