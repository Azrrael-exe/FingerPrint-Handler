SDK_INCLUDES = /opt/DigitalPersona/urusdk-linux/Include
LIB_OUT_DIR = /opt/DigitalPersona/urusdk-linux/Linux/lib
COMPILER = g++

main: src/main.cpp
	$(COMPILER) $? -I$(SDK_INCLUDES) -L $(LIB_OUT_DIR) -l dpfpdd -l dpfj -o bin/$@.o

test: src/test.cpp
	$(COMPILER) $? -I$(SDK_INCLUDES) -L $(LIB_OUT_DIR) -ldpfpdd -ldpfj -o bin/$@.o

capture: src/capture.cpp
	$(COMPILER) $? -I$(SDK_INCLUDES) -L $(LIB_OUT_DIR) -ldpfpdd -ldpfj -o bin/$@.o

read: src/read.cpp
	$(COMPILER) $? -I$(SDK_INCLUDES) -L $(LIB_OUT_DIR) -ldpfpdd -ldpfj -o bin/$@.o

clean:
	rm bin/*.o

wipe:
	rm data/*.bin
