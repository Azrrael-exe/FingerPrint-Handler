SDK_INCLUDES = /opt/DigitalPersona/urusdk-linux/Include
LIB_OUT_DIR = /opt/DigitalPersona/urusdk-linux/Linux/lib
COMPILER = g++

main: src/main.cpp
	$(COMPILER) $? -I$(SDK_INCLUDES) -L $(LIB_OUT_DIR) -ldpfpdd -ldpfj -o bin/$@.o

c: src/main.cpp
	$(CC) $? -I$(SDK_INCLUDES) -L $(LIB_OUT_DIR) -ldpfpdd -ldpfj -o bin/$@.o

clean:
	rm bin/*.o
