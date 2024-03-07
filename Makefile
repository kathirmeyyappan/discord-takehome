.PHONY: clean

main: main.cpp
	clang++ -g -o main main.cpp -lcriterion

clean:
	rm -rf a.out main main.dSYM