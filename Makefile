CC = gcc
CXX = g++

OUTPUT := g1m_export.exe
OBJS = main.o debug.o ../eternity_common/Utils.o ../eternity_common/BaseFile.o 
OBJS += ../eternity_common/crypto/sha1.o ../eternity_common/crypto/md5.o ../eternity_common/crypto/rijndael.o
OBJS += ../eternity_common/Stream.o ../eternity_common/MemoryStream.o ../eternity_common/FixedMemoryStream.o ../eternity_common/FileStream.o ../eternity_common/BitStream.o ../eternity_common/FixedBitStream.o
OBJS += ../eternity_common/math/Vector.o
OBJS += ../eternity_common/DOA6/G1mFile.o 
OBJS += ../eternity_common/tinyxml/tinyxml.o ../eternity_common/tinyxml/tinystr.o ../eternity_common/tinyxml/tinyxmlerror.o ../eternity_common/tinyxml/tinyxmlparser.o
CFLAGS=-Wall -I. -I../eternity_common -std=gnu99 -mno-ms-bitfields -O2 -masm=intel 
CFLAGS += -static
CXXFLAGS=-Wall -Wno-strict-aliasing -I../eternity_common -O2 -mno-ms-bitfields -std=c++11 -DTIXML_USE_STL 
LDFLAGS=-L. 
LIBS =  -lstdc++ -lversion -lz -ld3dx9

all: $(OUTPUT)

clean:
	rm -f $(OUTPUT) *.o
	rm -f ../eternity_common/*.o
	rm -f ../eternity_common/DOA6/*.o
	rm -f ../eternity_common/tinyxml/*.o
	rm -f ../eternity_common/crypto/*.o

$(OUTPUT): $(OBJS)
	$(LINK.c) $(LDFLAGS) -o $@ $^ $(LIBS)
