
all: bin tool tool_output bin/flct

bin/flct: src/fl_c_compiler.c tool_output/fl_c_gram.c tool_output/fl_c_lex.c
	time gcc -O2 -s -o bin/flct src/fl_c_compiler.c -Wall
	size bin/flct

tool_output/fl_c_gram.c: tool/lemon src/fl_c_gram.y
	./tool/lemon src/fl_c_gram.y -s -dtool_output -q
	sed -i 's/void Parse/static void Parse/g' tool_output/fl_c_gram.c

tool/lemon: tool/lemon.c tool/lempar.c
	gcc -O2 tool/lemon.c -o tool/lemon

tool/lemon.c:
	curl https://raw.githubusercontent.com/sqlite/sqlite/master/tool/lemon.c > tool/lemon.c

tool/lempar.c:
	curl https://raw.githubusercontent.com/sqlite/sqlite/master/tool/lempar.c > tool/lempar.c

tool_output/fl_c_lex.c: src/fl_c_lex.re
	re2c -W --eager-skip src/fl_c_lex.re -o tool_output/fl_c_lex.c

bin:
	mkdir bin

tool:
	mkdir tool

tool_output:
	mkdir tool_output

clean:
	rm -f bin/flct
	rm -f tool_output/fl_c_gram.c
	rm -f tool_output/fl_c_lex.c
