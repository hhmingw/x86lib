#Copyright (c) 2007 - 2010 Jordan "Earlz/hckr83" Earls  <http://www.Earlz.biz.tm>
#All rights reserved.

#Redistribution and use in source and binary forms, with or without
#modification, are permitted provided that the following conditions
#are met:

#1. Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
#2. Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
#3. The name of the author may not be used to endorse or promote products
#   derived from this software without specific prior written permission.
#   
#THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
#INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
#AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
#THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
#OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
#WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
#OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
#ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#This file is part of the x86Lib project.


HDRS =  include/config.h include/opcode_def.h include/x86lib.h include/x86lib_internal.h
CXX ?= g++
AR ?= ar


TEST_CC ?= i386-elf-gcc
TEST_CFLAGS ?= -fdata-sections -ffunction-sections


CXX_VM_SRC = vm/x86lib.cpp vm/modrm.cpp vm/device_manager.cpp vm/cpu_helpers.cpp vm/ops/strings.cpp vm/ops/store.cpp vm/ops/maths.cpp \
		  vm/ops/groups.cpp vm/ops/flow.cpp vm/ops/flags.cpp vm/ops/etc.cpp

CXX_VM_OBJS = $(subst .cpp,.o,$(CXX_VM_SRC))

CXX_TESTBENCH_SRC = testbench/testbench.cpp

CXX_TESTBENCH_OBJS = $(subst .cpp,.o,$(CXX_TESTBENCH_SRC))


CXXFLAGS ?= -Wall -g3 -fPIC
CXXFLAGS += -DX86LIB_BUILD -I./include -fexceptions

VERSION=1.1

VM_OUTPUTS = libx86lib.a
OUTPUTS = $(VM_OUTPUTS) x86testbench 

default: build

build: $(OUTPUTS)

libx86lib.a: $(CXX_VM_OBJS)
	ar crs libx86lib.a $(CXX_VM_OBJS)

x86testbench: $(CXX_TESTBENCH_OBJS) $(VM_OUTPUTS)
	$(CXX) $(CXXFLAGS) -o x86testbench $(CXX_TESTBENCH_OBJS) -lx86lib -L.

$(CXX_TESTBENCH_OBJS): $(HDRS) $(CXX_TESTBENCH_SRC)
	$(CXX) $(CXXFLAGS) -c $*.cpp -o $@

$(CXX_VM_OBJS): $(HDRS) $(CXX_VM_SRC)
	$(CXX) $(CXXFLAGS) -c $*.cpp -o $@

clean:
	rm -f $(CXX_VM_OBJS) $(OUTPUTS) $(CXX_TESTBENCH_OBJS)

buildtest:
	i386-elf-gcc -c testos.c -o testos.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra $(testos_CFLAGS)
	i386-elf-gcc -T linker.ld -o testos.bin -ffreestanding -O2 -nostdlib testos.o -lgcc -Wl,--gc-sections $(testos_CFLAGS) -dead_strip
	yasm -o testasm.bin testasm.asm

