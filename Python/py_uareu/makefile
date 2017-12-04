SDK_INCLUDES = /opt/DigitalPersona/urusdk-linux/Include
LIB_OUT_DIR = /opt/DigitalPersona/urusdk-linux/Linux/lib
PYTHON_INCLUDE = -I /usr/include/python2.7 -l python2.7
COMPILER = g++

u_are_u: u_are_u.h u_are_u.c _u_are_u.c
	$(COMPILER) -Wall -shared -fPIC $? $(PYTHON_INCLUDE) -I$(SDK_INCLUDES) -L$(LIB_OUT_DIR) -l dpfpdd -l dpfj -o $@.so

clean:
	rm *.so

# g++ -pthread -shared -Wl,-O1 -Wl,-Bsymbolic-functions -Wl,-Bsymbolic-functions -Wl,-z,relro -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 -Wall -Wstrict-prototypes -Wdate-time -D_FORTIFY_SOURCE=2 -g -fstack-protector-strong -Wformat -Werror=format-security -Wl,-Bsymbolic-functions -Wl,-z,relro -Wdate-time -D_FORTIFY_SOURCE=2 -g -fstack-protector-strong -Wformat -Werror=format-security build/temp.linux-x86_64-2.7/_u_are_u.o build/temp.linux-x86_64-2.7/u_are_u.o -L/opt/DigitalPersona/urusdk-linux/Linux/lib -ldpfpdd -ldpfj -o /home/azrrael/Dev/FingerPrint-Handler/uaru_py_wrapper/u_are_u.so
