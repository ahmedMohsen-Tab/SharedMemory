# --- Setup ---
CXX      := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -O2
LDTEST	 := -lgtest -lgtest_main -pthread

# --- Paths ---
TARGET    := $(CURDIR)/sharedMemoryTest

LIB_SOURCES := ${CURDIR}/main.cpp \
               ${CURDIR}/SemaphoreGuard.cpp \
               ${CURDIR}/SharedMemory.cpp

.PHONY: all build run clean buildDir

build:
	@echo "Building the project..."
	@${CXX} ${CXXFLAGS} ${LIB_SOURCES} -o ${TARGET} ${LDTEST}
	@${TARGET}

clean:
	@echo "cleaning ..."
	@rm -rf ${TARGET}
