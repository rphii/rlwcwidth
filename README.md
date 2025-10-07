# rlwcwidth

This project provides 

1. Estimation of character display widhts in terminals - the currently provided
   source-code is calculated for the `kitty` terminal
2. A fully automated way to re-calculate the character-display-widths

## Install

### Git

```sh
git clone https://github.com/rphii/rlwcwidth.git && cd rlwcwidth
meson setup build && meson install -C build
```

### Meson

\[$PROJECTROOT/subprojects/rlwcwidth.wrap\]
```meson.wrap
[wrap-git]
url = https://github.com/rphii/rlwcwidth.git
revision = main
```

\[$PROJECTROOT/meson.build\]
```meson.build
rlwcwidth_dep = dependency('rlwcwidth', fallback : ['rlwcwidth', 'rlwcwidth_dep'], default_options: ['default_library=static'])
```

## Why

Because somehow GNU's `wcwidth` didn't seem to work properly for me.

## How

See [run-all.sh](run-all.sh).

### Dependencies

To _"run-all"_ you need my other libraries:

- [rlc](https://www.github.com/rphii/rlc) - general purpose
- [rlso](https://www.github.com/rphii/rlso) - strings

### Stage 0 - [stage0-read-widths.c](stage0-read-widths.c)

TLDR: Estimate widths by printing all of 'em.

- The first stage prints all characters from UTF+0 until UTF+10FFFF.
- Before every print: move the cursor to (1,1) aka. top left.
- After every print: read out the cursor position.
- Once all characters are printed, dump acquired widhts into
  [rlwcwidth.lut.data.h](rlwcwidth.lut.data.h) in a comma-separated list.
- After this step we do have a working implementation that uses said dumped
  array data, see [rlwcwidth.lut.c](rlwcwidth.lut.c).

- However, I have some concerns that the resulting binary might be too large,
  hence we go to [stage1](#stage1)

### Stage 1 - [stage1-generate-if-else.c](stage1-generate-if-else.c)

TLDR: Compress data into if-else binary tree source code.

- From the implementation of the previous stage, break it into an array of
  `from..until`-ranges where there is a change of width.
- Generate a binary tree (`Node` struct) splitting the array above in half each
  time.
- Rearrange into a better format for the next step (`IfElse` struct)
- Output a final [rlwcwidth.c](rlwcwidth.c) _(+ header)_ which is essentially
  just that binary tree in an if-else arrangement.

### Crossvalidating Stage 0 with Stage 1

See [misc-comparison-test.c](misc-comparison-test.c).

