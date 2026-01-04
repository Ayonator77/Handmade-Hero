CXX = C:/msys64/mingw64/bin/g++.exe
CXXFLAGS = -g -Wall -fdiagnostics-color=always
LDFLAGS = -lgdi32
SRC = code/win32_handmade.cpp
OUT = build/win32_handmade.exe

all: $(OUT)

$(OUT): $(SRC)
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS)


run: $(OUT)
	$(OUT)

clean:
	del /Q $(OUT)
