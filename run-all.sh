#!/bin/bash
set -e

gcc stage0-read-widths.c -lrlso -o stage0.out
./stage0.out

reset

gcc stage1-generate-if-else.c rlwcwidth.lut.c -lrlso -lrlc -o stage1.out
./stage1.out

gcc misc-comparison-test.c rlwcwidth.lut.c rlwcwidth.c -o misc.out
./misc.out

