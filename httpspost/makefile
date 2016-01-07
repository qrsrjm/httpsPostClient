CC=arm-uclibc-gcc
#CC=gcc

FLAGS = -Wall -Werror -fPIC -g -shared -O2 -s -ldl -lm -o



LIB = -lstdc++     \
      -lssl

SSL_ARM_INCLUDE = ../openssl/arm/ssl/include
SSL_ARM_LIB = ../openssl/arm/ssl/lib
OBJ_ARM = libhttpspost_arm.so

SSL_X86_INCLUDE = ../openssl/x86/ssl/include
SSL_X86_LIB = ../openssl/x86/ssl/lib
OBJ_X86 = libhttpspost_x86.so


ifeq ($(CC),gcc)
SSL_INCLUDE = $(SSL_X86_INCLUDE)
SSL_LIB = $(SSL_X86_LIB)
OBJ = $(OBJ_X86)
else
SSL_INCLUDE = $(SSL_ARM_INCLUDE)
SSL_LIB = $(SSL_ARM_LIB)
OBJ = $(OBJ_ARM)
endif




SOURCE_FILE = sslSocket.cpp httpsPost.cpp

$(OBJ):$(SOURCE_FILE)
	$(CC) $(FLAGS)  $@  $(SOURCE_FILE) -I$(SSL_INCLUDE) $(SSL_LIB)/libssl.a $(SSL_LIB)/libcrypto.a

    
clean:
	rm -rf *.so

.PHONY: clean