# proto_gen_parsing

This will generate an .h file with prototypes for you based on all ".c" files in current directory. It places prototypes.h into protogen/ local directory.

Requires local directory protogen/, gcc, make, and re2c. If on Ubunutu intall build-essential and re2c.

Uses lemon, but the Makefile will take care of that for you.

Clone from git, then `$ make`

Browse the Makefile for more information.
