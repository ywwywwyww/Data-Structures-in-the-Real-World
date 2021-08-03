sSOURCES=$(wildcard *.cpp) $(wildcard *.cxx) $(wildcard *.cc)
HEADERS=$(wildcard *.h)
sOBJECTS1=$(sSOURCES:%.cpp=%.o)
sOBJECTS2=$(sOBJECTS1:%.cxx=%.o)
sOBJECTS=$(sOBJECTS2:%.cc=%.o)
TARGET=$(shell pwd |sed 's/^\(.*\)[/]//' )
COMPILEOPTION=-std=c++17 -lpthread -O3 -mavx2


all: $(TARGET)
$(TARGET): $(sOBJECTS) $(HEADERS) $(LIB)
	@echo $(sOBJECTS)
	@echo "Now Generating $(TARGET) ..."
	g++ $(sOBJECTS) $(LIB) -o $(TARGET) $(COMPILEOPTION)
%.o: %.cpp $(HEADERS)
	@echo "Now Compiling $< ..."
	g++ -c $< -o $@ $(COMPILEOPTION)
%.o: %.cxx $(HEADERS)
	@echo "Now Compiling $< ..."
	g++ -c $< -o $@ $(COMPILEOPTION)
%.o: %.cc $(HEADERS)
	@echo "Now Compiling $< ..."
	g++ -c $< -o $@ $(COMPILEOPTION)
clean:
	rm -f $(TARGET) $(sOBJECTS)
explain:
	@echo "User Sources: $(sSOURCES)"
	@echo "User Objects: $(sOBJECTS)"
	@echo "Target: $(TARGET)"
	@echo "Compile Option: $(COMPILEOPTION)"
