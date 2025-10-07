project('rlwcwidth', 'c', meson_version: '>=1.1', version: '9999')

sources = [
  'rlwcwidth.c'
  ]

install_headers('rlwcwidth.h')

librlwcwidth = library('rlwcwidth',
  sources,
  install: true,
  )

pkg = import('pkgconfig')
pkg.generate(
  libraries: librlwcwidth,
  version: '9999',
  name: 'rlwcwidth',
  description: 'TUI helpers for rl*',
  )


rlwcwidth_dep = declare_dependency(
  link_with: librlwcwidth,
  include_directories: '.',
  )
