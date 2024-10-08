INSTALLDIR=$(CURDIR)/libcarla-install
INCDIR=$(INSTALLDIR)/include
BUILDDIR=$(CURDIR)/build

CC=/usr/bin/gcc
CXX=/usr/bin/g++
CXXFLAGS=-std=c++17 -pthread -fPIC  -O3 -DNDEBUG -Werror -Wall -Wextra

define log
	@echo "\033[1;35m$(1)\033[0m"
endef

bridge: $(BUILDDIR)/bridge
	$(call log,Running carla_zenoh_bridge.cpp...)
	@$(BUILDDIR)/bridge $(ARGS)

$(BUILDDIR)/bridge: carla-zenoh-bridge.cpp Connection.cpp Connection.hpp listeners.cpp listeners.hpp
	$(call log,Compiling source files...)
	@$(CXX) $(CXXFLAGS) -I $(INCDIR) -isystem $(INCDIR)/system \
		-L$(INSTALLDIR)/lib -L/usr/local/lib/ \
		-o $(BUILDDIR)/bridge carla-zenoh-bridge.cpp listeners.cpp Connection.cpp \
		-Wl,-Bstatic -lcarla_client -lrpc -lboost_filesystem \
		-Wl,-Bdynamic -lpng -ltiff -ljpeg -lRecast -lDetour -lDetourCrowd -lzenohc


controls: $(BUILDDIR)/control_data
	$(call log,Running control_data.cpp...)
	@$(BUILDDIR)/control_data $(ARGS)

$(BUILDDIR)/control_data: control_data.cpp
	$(call log,Compiling control_data.cpp...)
	@mkdir -p $(BUILDDIR)
	@$(CXX) $(CXXFLAGS) -I $(INCDIR) \
	-L/usr/local/lib \
	-o $(BUILDDIR)/control_data control_data.cpp \
	-lzenohc




spawn: $(BUILDDIR)/spawner
	$(call log,Running spawner.cpp...)
	@$(BUILDDIR)/spawner $(ARGS)

$(BUILDDIR)/spawner: spawner.cpp
	$(call log,Compiling spawner.cpp...)
	@mkdir -p $(BUILDDIR)
	@$(CXX) $(CXXFLAGS) -I $(INCDIR) -isystem $(INCDIR)/system -L $(INSTALLDIR)/lib \
		-o $(BUILDDIR)/spawner spawner.cpp \
		-Wl,-Bstatic -lcarla_client -lrpc -lboost_filesystem -Wl,-Bdynamic \
		-lpng -ltiff -ljpeg -lRecast -lDetour -lDetourCrowd


test: $(BUILDDIR)/test_listener
	$(call log, Running test_listener...)
	@$(BUILDDIR)/test_listener $(ARGS)

$(BUILDDIR)/test_listener: test_listener.cpp
	$(call log,Compiling test_listener.cpp...)
	@$(CXX) $(CXXFLAGS) -I $(INCDIR) -isystem $(INCDIR)/system -L $(INSTALLDIR)/lib \
		-o $(BUILDDIR)/test_listener test_listener.cpp \
		-Wl,-Bstatic -lcarla_client -lrpc -lboost_filesystem -Wl,-Bdynamic \
		-lpng -ltiff -ljpeg -lRecast -lDetour -lDetourCrowd -lzenohc


.PHONY: clean
clean:
	$(call log,Removing build directory...)
	@rm -rf $(BUILDDIR)
