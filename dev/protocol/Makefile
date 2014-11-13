#------------------------------------------------------------------------------
# .proto compilation script 
#------------------------------------------------------------------------------
INDIR  := src
OUTDIR := compiled

PROTOFILES := $(shell find $(INDIR)/ -type f -name '*.proto')
FILES := $(patsubst $(INDIR)/%.proto, %, $(PROTOFILES))
CCFILES := $(addprefix $(OUTDIR)/,$(FILES:=.pb.cc))

#------------------------------------------------------------------------------
all: $(CCFILES)
#------------------------------------------------------------------------------
.PHONY: clean
#------------------------------------------------------------------------------
clean:
	@echo Cleaning...
	@rm -fr compiled

#------------------------------------------------------------------------------
$(OUTDIR)/%.pb.cc: $(INDIR)/%.proto
	@mkdir -p "$(@D)"
	@echo - $(notdir $<)
	@cd src;\
	protoc --cpp_out=../$(OUTDIR) $(patsubst src/%,%,$<)
