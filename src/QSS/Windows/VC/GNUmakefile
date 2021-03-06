# QSS Make File
#
# Language: GNU Make
#
# Platform: Windows/VC

# Paths
PKG_PATH := $(QSS)\src\QSS
SRC_PATH := $(PKG_PATH)
INC_PATH := $(INCLUDE)
TST_PATH := $(QSS)\tst\QSS\unit

# Initialization
include $(QSS_bin)/GNUmakeinit.mk

# Variables
TAR := QSS
SRC := $(filter-out %.in.cc,$(notdir $(wildcard $(SRC_PATH)/*.cc)))
SRC += $(filter-out %.in.cc,$(notdir $(wildcard $(SRC_PATH)/dfn/*.cc)))
SRC += $(filter-out %.in.cc,$(notdir $(wildcard $(SRC_PATH)/dfn/mdl/*.cc)))
SRC += $(filter-out %.in.cc,$(notdir $(wildcard $(SRC_PATH)/fmu/*.cc)))
DEP := $(addsuffix .d,$(basename $(SRC)))
OBJ := $(addsuffix .obj,$(basename $(SRC)))
SLB := $(BIN_PATH)\lib$(TAR).lib
EXE := $(BIN_PATH)\$(TAR).exe

# Rules

.PHONY : all
all : $(EXE) $(SLB)

.PHONY : exe
exe : $(EXE)

.PHONY : lib
lib : $(SLB)

.PHONY : dep
dep : $(DEP)

.PHONY : obj
obj : $(OBJ)

.PHONY : check
check :
	@echo $(TAR)
	@echo $(SRC)
	@echo $(DEP)
	@echo $(OBJ)
	@echo $(SLB)
	@echo $(EXE)

.PHONY : clear
clear :
	-rm -f *.d *.obj

.PHONY : clean
clean :
	-rm -f *.d *.obj $(subst \,/,$(SLB)) $(subst \,/,$(EXE)) $(subst \,/,$(BIN_PATH))/$(TAR).ilk $(subst \,/,$(BIN_PATH))/$(TAR).pdb

# Library from objects
$(SLB) : $(filter-out QSS.obj,$(OBJ))
	@-rm -f $(filter-out $(DEP),$(wildcard *.d)) # Prune obs deps
	@-rm -f $(filter-out $(OBJ),$(wildcard *.obj)) # Prune obs objs
	$(AR) $(ARFLAGS) /out:$@ $?

# Executable from objects
$(EXE) : $(OBJ)
	@-rm -f $(filter-out $(DEP),$(wildcard *.d)) # Prune obs deps
	@-rm -f $(filter-out $(OBJ),$(wildcard *.obj)) # Prune obs objs
	$(CXX) $(LDFLAGS) /Fe:$@ $^ $(LINKFLAGS) $(FMIL_HOME)\lib\fmilib.lib
	rm -f $(subst \,/,$(BIN_PATH))/$(TAR).exp $(subst \,/,$(BIN_PATH))/$(TAR).lib

# Dependencies
ifneq ($(MAKECMDGOALS),dep)
ifneq ($(MAKECMDGOALS),check)
ifneq ($(MAKECMDGOALS),clear)
ifneq ($(MAKECMDGOALS),clean)
-include $(DEP)
endif
endif
endif
endif
