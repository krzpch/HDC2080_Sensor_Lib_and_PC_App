# Cpputest makefile
# Author: Konrad Sikora

# Silent build
SILENCE = @

# Cpputest directory
CPPUTEST_HOME = ../cpputest/

include unit_tests_files.mk

# Project home directory
PROJECT_HOME_DIR = ../../HDC2080/HDC2080_lib/

# Output file name
COMPONENT_NAME = hdc2080_ut

# CppUTest config
CPPUTEST_USE_EXTENSIONS = Y
CPPUTEST_USE_GCOV = Y
CPPUTEST_WARNINGFLAGS = -Wall

CPPUTEST_OBJS_DIR = test-obj
CPPUTEST_LIB_DIR = test-lib

include $(CPPUTEST_HOME)/build/MakefileWorker.mk
