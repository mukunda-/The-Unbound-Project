#------------------------------------------------------------------------------
# .proto compilation script 
#------------------------------------------------------------------------------
OUTDIR := compiled/proto

PROTOFILES := $(shell find src/ -type f -name '*.proto')
FILES := $(patsubst src/%.proto, %, $(PROTOFILES))

CCFILES := $(FILES:=.pb.cc)
CCFILES := $(addprefix $(OUTDIR)/,$(CCFILES))

all: folder $(CCFILES)
.PHONY: folder clean

folder:

clean:
	@echo Cleaning...
	@rm -fr compiled

$(OUTDIR)/%.pb.cc: src/%.proto
	@mkdir -p "$(@D)"
	@echo - $(notdir $<)
	@cd src;\
	protoc --cpp_out=../$(OUTDIR) $(patsubst src/%,%,$<)
