
all: bin tool tool_output gen bin/flct

bin/flct: src/fl_c_compiler.c tool_output/fl_c_gram.c tool_output/fl_c_lex.c gen/prototypes.h
	time gcc -O2 -s -o bin/flct src/fl_c_compiler.c -Wall
	size bin/flct

tool_output/fl_c_gram.c: tool/lemon src/fl_c_gram.y
	./tool/lemon src/fl_c_gram.y -s -dtool_output
	sed -i 's/void Parse/static void Parse/g' tool_output/fl_c_gram.c

tool/lemon: tool/lemon.c tool/lempar.c
	gcc -O2 tool/lemon.c -o tool/lemon

tool/lemon.c:
	curl https://raw.githubusercontent.com/sqlite/sqlite/master/tool/lemon.c > tool/lemon.c

tool/lempar.c:
	curl https://raw.githubusercontent.com/sqlite/sqlite/master/tool/lempar.c > tool/lempar.c

tool_output/fl_c_lex.c: src/fl_c_lex.re
	re2c -W --eager-skip src/fl_c_lex.re -o tool_output/fl_c_lex.c

gen/prototypes.h: src/*.c prototype_generator/bin/protoGenCompiler
	./prototype_generator/bin/protoGenCompiler

prototype_generator/bin/protoGenCompiler:
	git clone --depth 1 --quiet https://github.com/cornerStone-Dev/prototype_generator.git
	(cd prototype_generator && make)

bin:
	mkdir bin

tool:
	mkdir tool

tool_output:
	mkdir tool_output

gen:
	mkdir gen

clean:
	rm -f bin/flct
	rm -f tool_output/fl_c_gram.c
	rm -f tool_output/fl_c_lex.c
