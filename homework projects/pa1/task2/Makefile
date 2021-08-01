sSOURCES=$(wildcard *.cpp) $(wildcard *.cxx) $(wildcard *.cc)
HEADERS=$(wildcard *.h)
sOBJECTS1=$(sSOURCES:%.cpp=%.o)
sOBJECTS2=$(sOBJECTS1:%.cxx=%.o)
sOBJECTS=$(sOBJECTS2:%.cc=%.o)
TARGET=$(shell pwd |sed 's/^\(.*\)[/]//' )


all: $(TARGET)
$(TARGET): $(sOBJECTS) $(HEADERS) $(LIB)
	@echo $(sOBJECTS)
	@echo "Now Generating $(TARGET) ..."
	g++ $(sOBJECTS) $(LIB) -o $(TARGET) -std=c++17 -O3
%.o: %.cpp $(HEADERS)
	@echo "Now Compiling $< ..."
	g++ -c $< -o $@ -std=c++17 -O3
%.o: %.cxx $(HEADERS)
	@echo "Now Compiling $< ..."
	g++ -c $< -o $@ -std=c++17 -O3
%.o: %.cc $(HEADERS)
	@echo "Now Compiling $< ..."
	g++ -c $< -o $@ -std=c++17 -O3
clean:
	rm -f $(TARGET) $(sOBJECTS)
explain:
	@echo "User Sources: $(sSOURCES)"
	@echo "User Objects: $(sOBJECTS)"
	@echo "Target: $(TARGET)"
