# florida_c

This project is a translator for a dialect of the C programming language.

To build the project requires gcc, make, and re2c. If on Ubunutu intall build-essential and re2c.

Uses lemon, but the Makefile will take care of that for you.

Clone from git, then `$ make`

Browse the Wiki for more details on florida_c.

Browse the Makefile for more information on building.

## Usage

Currently florida_c has one command line option, -dDIR_NAME where you can specify the output directory. The default is "c_src".
