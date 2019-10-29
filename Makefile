.PHONY: buildir, clean

CPPFLAGS += -std=c++17

compile: buildir
	$(CXX) asm.cpp -o build/asm $(CPPFLAGS) $(LDFLAGS)

clean:
	rm -rf build

buildir:
	mkdir -p build
