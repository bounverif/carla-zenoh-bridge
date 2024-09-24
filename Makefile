INSTALLDIR=$(CURDIR)/libcarla-install
INCDIR=$(INSTALLDIR)/include
BUILDDIR=$(CURDIR)/build

CC=/usr/bin/gcc
CXX=/usr/bin/g++
CXXFLAGS=-std=c++14 -pthread -fPIC  -O3 -DNDEBUG -Werror -Wall -Wextra

define log
	@echo "\033[1;35m$(1)\033[0m"
endef

clean:
	@rm -rf $(BUILDDIR)

controls: run-controls

run-controls: build-controls
	$(call log,Running control_data.cpp...)
	@$(BUILDDIR)/control_data $(ARGS)

build-controls:
	$(call log,Compiling control_data.cpp...)
	@mkdir -p $(BUILDDIR)
	@$(CXX) $(CXXFLAGS) -I $(INCDIR) -o $(BUILDDIR)/control_data control_data.cpp




test: run-test

run-test: build-test
	$(call log,Running test.cpp...)
	@$(BUILDDIR)/test $(ARGS)

build-test:
	$(call log,Compiling test.cpp...)
	@mkdir -p $(BUILDDIR)
	@$(CXX) $(CXXFLAGS) -I $(INCDIR) -isystem $(INCDIR)/system -L $(INSTALLDIR)/lib \
		-o $(BUILDDIR)/test test.cpp \
		-Wl,-Bstatic -lcarla_client -lrpc -lboost_filesystem -Wl,-Bdynamic \
		-lpng -ltiff -ljpeg -lRecast -lDetour -lDetourCrowd
