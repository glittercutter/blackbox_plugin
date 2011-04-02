OBJDIR = ./o/mingw
SRCDIR = ./src

# Automatic dependency
DEPS := $(wildcard $(OBJDIR)/*.d)

EXE = bbDynamicTiling.dll

CFLAGS	= -Wall -pedantic -Werror -pipe -std=c++0x
LFLAGS	= -mwindows -shared
LIBS	= -lblackbox -lbblib -lversion -lkernel32 -luser32 -lgdi32

ifeq ($(build_type), debug)
	CFLAGS += -O0 -g #-pg
	LFLAGS += -g #-pg
else
	CFLAGS += -D NDEBUG -Os -fno-rtti -fno-exceptions
	LFLAGS += -s -fno-rtti -fno-exceptions
endif

CC = g++ -MD
DO_CC=$(CC) $(CFLAGS) -o $@ -c $<

# top-level rules
all : create_dir $(EXE)

# create missing directory
create_dir :
	@test -d $(OBJDIR) || mkdir -p $(OBJDIR)

############################################################################
# FILES
############################################################################

OBJ = \
	$(OBJDIR)/bbDT.o \
	$(OBJDIR)/bbDT_Client.o \
	$(OBJDIR)/bbDT_Column.o \
	$(OBJDIR)/bbDT_Container.o \
	$(OBJDIR)/bbDT_Manager.o \
	$(OBJDIR)/bbDT_Workspace.o \

$(OBJDIR)/bbDT.o : $(SRCDIR)/bbDT.cpp; $(DO_CC)
$(OBJDIR)/bbDT_Client.o : $(SRCDIR)/bbDT_Client.cpp; $(DO_CC)
$(OBJDIR)/bbDT_Column.o : $(SRCDIR)/bbDT_Column.cpp; $(DO_CC)
$(OBJDIR)/bbDT_Container.o : $(SRCDIR)/bbDT_Container.cpp; $(DO_CC)
$(OBJDIR)/bbDT_Manager.o : $(SRCDIR)/bbDT_Manager.cpp; $(DO_CC)
$(OBJDIR)/bbDT_Workspace.o : $(SRCDIR)/bbDT_Workspace.cpp; $(DO_CC)

###########################################################################

# Automatic dependency
-include $(DEPS)

$(EXE) : $(OBJ)
	$(CC) $(OBJ) -o $(EXE) $(LFLAGS) $(LIBS)

clean:
	rm $(OBJDIR)*.o
# DO NOT DELETE
