# ECE581-LLC-GRP1
This is group 1's implementation of an LLC (Last Level Cache)

## Introduction
This repo contains the design of a Last Level Cache by Team 1 for ECE-585 Spring 2024.

The goal of this project was to create a LLC simulator that implemented the MESI protocol that implemented write allocate policy along with
cache coherency through messaging to the L1 cache above it. Our cache needs to support 16MB of capacity with 64-byte lines and be
16-way associative.  We also have to snoop bus operations and place bus operations on the shared LLC bus. The L1 Cache that we communicate 
with employes a write through once and write-back subsequentially. Our cache should be configurable to different levels of capacity and associativity.

The layout of this project includes a src, inc, docs, and testing directories.

|

|--src

|--include

|--doc

|--testing

src -- source files and modules needed to build our LLC simulator.

include -- headers and includes for simulator and testing.

doc -- [docs](doc/README.md) folder with complete design and testing implementation details.

testing -- testing folder with generated and hand written tests.

This README will also provide a tutorial on how to build and run the system.

## Building and Running the Project
To run this test you must be inside the src directory using the provided Makefile.

The following steps can be taken to build the project:

`mkdir obj`

`make clean`

`make build`

NOTE: If you want to set debug mode run `make build DEBUG=true`

NOTE: We have not figured out how to create the object file directly in the makefile for now

After running the build command the executable `llc` will be placed into the `bin/` under the `src/` directory.

You can then run the command using the following structure: `./src/bin/llc <path/to/test> -<mode>`

If you do not supply a `mode` the system will default to nomralMode. If you don't pass a path it defautls to
`rwims.din` inside the repo.

## Options for the Command Line
`-n` -- Normal Mode printout for the system

`-s` -- Silent Mode printout for the system

## Adendum
There are certain variables that can be set in the makefile to change at build time with the -D flag, those can
be found in the defines.h. If a variable is not set in the defines.h header, it is not meant to be changed between
operations.
