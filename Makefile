LDFLAGS		:= -lpthread
CXXFLAGS	:= -O3

all: ircs

ircs: ircs.cpp usuarios.o canales.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	
clean:
	$(RM) *.o *~ ircs
