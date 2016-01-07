CC=arm-uclibc-gcc
#CC=gcc

FLAGS = -Wall -Werror -fPIC -g -O2 -s -ldl -lm -o


SC_LIBRARY_PATH = ../lib

SOURCE_FILE = exsample.cpp

ifeq ($(CC),gcc)
SSL_INCLUDE = ../inc
OBJ = exsample_x86
LIB = -lstdc++     \
      -lhttpspost_x86
else
SSL_INCLUDE = ../inc
OBJ = exsample_arm
LIB = -lstdc++     \
      -lhttpspost_arm
endif

    
$(OBJ): $(SOURCE_FILE)
	$(CC) $(FLAGS)  $@  $(SOURCE_FILE) -I$(SSL_INCLUDE) -L$(SC_LIBRARY_PATH) $(LIB)

    
#SERVER_SOURCE_FILE = server.c
#ssl_server: $(SERVER_SOURCE_FILE)
#	$(CC) $(FLAGS)  $@  $(SERVER_SOURCE_FILE) $(LIB)
    
    
clean:
	rm -rf $(OBJ)
	rm -rf ssl_server

.PHONY: clean