# GBA Homebrew Makefile (devkitARM + libtonc)
TARGET   := pokemon_uttarakhand
CC       := $(DEVKITARM)/bin/arm-none-eabi-gcc
OBJCOPY  := $(DEVKITARM)/bin/arm-none-eabi-objcopy
GBAFIX   := $(DEVKITPRO)/tools/bin/gbafix

CFLAGS   := -mthumb -mthumb-interwork -O2 -Wall -fno-strict-aliasing \
            -I$(DEVKITPRO)/libtonc/include
LDFLAGS  := -mthumb -mthumb-interwork -specs=gba.specs \
            -L$(DEVKITPRO)/libtonc/lib -ltonc

all: $(TARGET).gba

$(TARGET).elf: main.c
	$(CC) $(CFLAGS) main.c $(LDFLAGS) -o $@

$(TARGET).gba: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@
	$(GBAFIX) $@ -t"PKMN UKHAND" -c"PUKE" -m"01" -r0

clean:
	rm -f $(TARGET).elf $(TARGET).gba
