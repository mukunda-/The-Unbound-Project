#------------------------------------------------------------------------------
# .proto compilation script 
#------------------------------------------------------------------------------
OUTDIR := compiled

PROTOFILES := $(notdir $(wildcard $(CURDIR)/*.proto))
CCFILES := $(basename $(PROTOFILES))
CCFILES := $(addprefix $(OUTDIR)/,$(CCFILES:=.pb.cc))

all: folder $(CCFILES)

.PHONY: folder clean

folder:
	@mkdir -p $(OUTDIR)

clean:
	@echo Cleaning...
	@rm -fr compiled

$(CCFILES) : $(OUTDIR)/%.pb.cc : %.proto
	@echo - $(notdir $<)
	@protoc --cpp_out=$(OUTDIR) $<
