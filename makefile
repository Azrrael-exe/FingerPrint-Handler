SDK_INCLUDES = /opt/DigitalPersona/urusdk-linux/Include
LIB_OUT_DIR ?= /opt/DigitalPersona/urusdk-linux/Linux/lib

main: src/main.cpp
	$(CC) $? -I$(SDK_INCLUDES) -L $(LIB_OUT_DIR) -ldpfpdd -ldpfj -o bin/$@.o

clean:
	rm bin/*.o
