SRCDIR    := common
BINDIR    := bin
OBJDIR    := obj

BIN       := $(BINDIR)/soapine-x86_64
LIBGNUEFI := gnu-efi/x86_64/gnuefi/libgnuefi.a

CC        := x86_64-elf-gcc
LD        := x86_64-elf-ld
OBJCOPY   := x86_64-elf-objcopy

CFLAGS := \
	-fpic \
	-ffreestanding \
	-fno-stack-protector \
	-fno-stack-check \
	-fshort-wchar \
	-mno-red-zone \
	-maccumulate-outgoing-args

CPPFLAGS := \
	-Ignu-efi/inc \
	-I$(SRCDIR) \
	-MMD

LDFLAGS := \
	-shared \
	-Bsymbolic \
	-Lgnu-efi/x86_64/lib \
	-Lgnu-efi/x86_64/gnuefi \
	-Tgnu-efi/gnuefi/elf_x86_64_efi.lds \
	gnu-efi/x86_64/gnuefi/crt0-efi-x86_64.o
	
LDLIBS := \
	-lgnuefi \
	-lefi

OBJCOPYFLAGS := \
	-j .text \
	-j .sdata \
	-j .data \
	-j .rodata \
	-j .dynamic \
	-j .dynsym \
	-j .rel \
	-j .rela \
	-j .rel.* \
	-j .rela.* \
	-j .reloc \
	--target efi-app-x86_64 \
	--subsystem=10

SRCS_C = $(shell find $(SRCDIR) -name '*.c')
OBJS_C = $(SRCS_C:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
DEPS_C = $(OBJS_C:$(OBJDIR)/%.o=$(OBJDIR)/%.o.d)

all: $(BIN).efi

$(BIN).efi: $(LIBGNUEFI) $(OBJS_C) 
	@mkdir -p $(BINDIR)

	@echo "    LD       $(BIN).so"
	@$(LD) $(LDFLAGS) $(OBJS_C) -o $(BIN).so $(LDLIBS)

	@echo "    OBJCOPY  $(BIN).efi"
	@$(OBJCOPY) $(OBJCOPYFLAGS) $(BIN).so $(BIN).efi

include $(wildcard $(OBJDIR)/*.d)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(@D)
	@echo "    CC       $<"
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@
#	@$(CC) -MM $(CFLAGS) $(CPPFLAGS) $< > $@.d

$(LIBGNUEFI):
	@git clone https://git.code.sf.net/p/gnu-efi/code gnu-efi
	@make -C gnu-efi

clean:
	@rm -rf obj/
	@rm -rf bin/
	@rm -rf gnu-efi/
	@rm -rf ovmf/

include ./run.mk