# Cpputest list of source files for build
# Author: Konrad Sikora

# List of C sources files
SRC_FILES += $(PROJECT_HOME_DIR)/HDC2080.c
# SRC_DIRS += 

# List of C testing files
TEST_SRC_FILES += run.cpp
TEST_SRC_FILES += test_hdc2080.c
# TEST_SRC_DIRS +=

# List of C mock files
MOCKS_SRC_DIRS += mocks/

# List of locations of header files
INCLUDE_DIRS += $(CPPUTEST_HOME)/include
INCLUDE_DIRS += $(CPPUTEST_HOME)/include/Platforms/Gcc
INCLUDE_DIRS += $(PROJECT_HOME_DIR)
INCLUDE_DIRS += stubs/
