INSTALLDIR=$(CURDIR)/libcarla-install
INCDIR=$(INSTALLDIR)/include
BUILDDIR=$(CURDIR)/build

CC=/usr/bin/gcc
CXX=/usr/bin/g++
CXXFLAGS=-std=c++17 -pthread -fPIC  -O3 -DNDEBUG -Werror -Wall -Wextra

define log
	@echo "\033[1;35m$(1)\033[0m"
endef

clean:
	@rm -rf $(BUILDDIR)


bridge: run-bridge

run-bridge: build-bridge
	$(call log,Running carla_zenoh_bridge.cpp...)
	@$(BUILDDIR)/bridge $(ARGS)

build-bridge: carla_zenoh_bridge.cpp VehicleSim.cpp VehicleSim.hpp
	$(call log,Compiling source files...)
	@$(CXX) $(CXXFLAGS) -I $(INCDIR) -isystem $(INCDIR)/system \
		-L$(INSTALLDIR)/lib -L/usr/local/lib/ \
		-o $(BUILDDIR)/bridge carla_zenoh_bridge.cpp VehicleSim.cpp \
		-Wl,-Bstatic -lcarla_client -lrpc -lboost_filesystem \
		-Wl,-Bdynamic -lpng -ltiff -ljpeg -lRecast -lDetour -lDetourCrowd -lzenohc


controls: run-controls

run-controls: build-controls
	$(call log,Running control_data.cpp...)
	@$(BUILDDIR)/control_data $(ARGS)

build-controls: control_data.cpp
	$(call log,Compiling control_data.cpp...)
	@mkdir -p $(BUILDDIR)
	@$(CXX) $(CXXFLAGS) -I $(INCDIR) \
	-L/usr/local/lib \
	-o $(BUILDDIR)/control_data control_data.cpp \
	-lzenohc



spawn: run-spawner

run-spawner: build-spawner
	$(call log,Running spawner.cpp...)
	@$(BUILDDIR)/spawner $(ARGS)

build-spawner: spawner.cpp
	$(call log,Compiling spawner.cpp...)
	@mkdir -p $(BUILDDIR)
	@$(CXX) $(CXXFLAGS) -I $(INCDIR) -isystem $(INCDIR)/system -L $(INSTALLDIR)/lib \
		-o $(BUILDDIR)/spawner spawner.cpp \
		-Wl,-Bstatic -lcarla_client -lrpc -lboost_filesystem -Wl,-Bdynamic \
		-lpng -ltiff -ljpeg -lRecast -lDetour -lDetourCrowd
