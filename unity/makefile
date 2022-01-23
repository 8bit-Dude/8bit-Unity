# Include paths we may need for compilations
ifeq ($(CC65_INC),)
	CC65_INC=/usr/share/cc65/include
endif
ifeq ($(CA65_INC),)
	CA65_INC=/usr/share/cc65/asminc
endif
ifeq ($(BUILD),)
	BUILD=debug
 endif

# Compiling for Atari Lynx system
SYS=lynx

# Names of tools
CO=co65
CC=cc65
AS=ca65
AR=ar65
CL=cl65
CP=cp
RM=rm -f
ECHO=echo
TOUCH=touch

# Flag for assembler
AFLAGS=-I $(CA65_INC) -t $(SYS) --cpu 65SC02 -D $(BUILD) -D __HUB__ $(SYMBOLS)

# Flags for C-code compiler
CFLAGS=-I $(CC65_INC) -Cl -O -t $(SYS) --cpu 65SC02 -D $(BUILD) -D __HUB__  

#lynx
OBJECTS = targets/lynx/serial.o adaptors/hub.o network/net-web.o network/net-ip.o network/net-url.o network/net-base.o network/net-easy.o network/net-tcp.o network/net-udp.o 

#c64-hub			
#OBJECTS = targets/c64/CIA.o adaptors/hub.o network/net-url.o network/net-base.o network/net-easy.o network/net-tcp.o network/net-udp.o 

#c64-ip65			
#OBJECTS = targets/c64/CIA.o network/net-url.o network/net-base.o network/net-easy.o network/net-tcp.o network/net-udp.o 

#atmos-hub			
#OBJECTS = targets/oric/VIA.o adaptors/hub.o network/net-base.o network/net-easy.o network/net-tcp.o network/net-udp.o 

#apple2-hub			
#OBJECTS = targets/apple2/clock.o adaptors/hub.o network/net-url.o network/net-base.o network/net-easy.o network/net-tcp.o network/net-udp.o 

#atari			
#OBJECTS = targets/atari/PIA.o adaptors/hub.o network/net-url.o network/net-base.o network/net-easy.o network/net-tcp.o network/net-udp.o 

#nes	
#OBJECTS = targets/nes/crt0.o targets/nes/expansion.o network/net-url.o adaptors/hub.o network/net-base.o network/net-easy.o network/net-tcp.o network/net-udp.o 

TARGET=unity-network-$(SYS).lib 

$(TARGET) : $(OBJECTS)
	$(AR) a $@ $? 

# Rule for making a *.o file out of a *.c file
%.o: %.c
	$(CC) $(CFLAGS) $(SEGMENTS) --create-dep $(@:.o=.d) --dep-target $@ -o $(@:.o=.s) $<
	$(AS) -t $(SYS) -o $@ $(@:.o=.s)
	$(RM) $*.s

# Rule for making a *.o file out of a *.s or *.asm file
%.o: %.s
	$(AS) -o $@ $(AFLAGS) $<
%.o: %.asm
	$(AS) -o $@ $(AFLAGS) $<

all: $(TARGET)

clean :
	$(RM) $(OBJECTS)
