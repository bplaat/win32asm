# bplaat/win32asm
Pure 32-bit and 64-bit win32 assembly windows programs and some gcc compiler to assembly custom win32 header stuff

To assemble the programs you need the [NASM](https://nasm.us/) assembler. All headers are generated via macro's so you can compile on every platform and don't need specific Windows linkers. You could run the [`build.sh`](build.sh) script to assemble all the programs.

In the folder `custom-gcc` is a little project which uses the gcc compiler to compile c code to assembley which then is assembled with custom headers via nasm. This gives you in essence the best of both worlds, but it is still experimental.
