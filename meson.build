project(
	'sdc',
	'c',
	version: '0.1.0',
	license: 'MIT',
	meson_version: '>=0.49.0',
	default_options: [
		'c_std=c11',
		'warning_level=2',
		'werror=true',
	],
)

cc = meson.get_compiler('c')

add_project_arguments(cc.get_supported_arguments([
	'-Wundef',
	'-Wlogical-op',
	'-Wmissing-include-dirs',
	'-Wold-style-definition',
	'-Wpointer-arith',
	'-Winit-self',
	'-Wstrict-prototypes',
	'-Wimplicit-fallthrough=2',
	'-Wendif-labels',
	'-Wstrict-aliasing=2',
	'-Woverflow',
	'-Wno-missing-braces',
	'-Wno-missing-field-initializers',
	'-Wno-unused-parameter',
	'-O3'
]), language: 'c')

csfmlwindow = cc.find_library('libcsfml-window', dirs : ['/usr/lib'])
csfmlsystem = cc.find_library('libcsfml-system', dirs : ['/usr/lib'])
csfmlgraphics = cc.find_library('libcsfml-graphics', dirs : ['/usr/lib'])

sdc = executable('sdc',
	files(
		'src/game.c',
		'src/inputhandler.c',
		'src/main.c',
	),
	dependencies: [
		csfmlgraphics,
		csfmlsystem,
		csfmlwindow,
	],
	include_directories: include_directories('include'),
)
