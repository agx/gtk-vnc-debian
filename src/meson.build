
gvnc_marshal = gnome.genmarshal(
  'vncmarshal',
  sources: ['vncmarshal.txt'],
  install_header: false,
)

gvnc_enum_headers = gnome.mkenums(
  'vncconnectionenums.h',
  sources: ['vncconnection.h'],
  comments: '/* @comment@ */',
  fhead: '#ifndef VNC_CONNECTION_ENUMS_H\n#define VNC_CONNECTION_ENUMS_H\n\n#include <glib-object.h>\nG_BEGIN_DECLS\n\n',
  ftail: 'G_END_DECLS\n\n#endif /* VNC_CONNECTION_ENUMS_H */\n',
  fprod: '\n/* --- vncconnection.h --- */',
  eprod: '#define VNC_TYPE_@ENUMSHORT@ @enum_name@_get_type()\nGType @enum_name@_get_type (void);\n',
  install_header: true,
  install_dir: join_paths(gtk_vnc_includedir, 'gvnc-1.0'),
)

gvnc_enum_headers_dep = declare_dependency(
  sources: gvnc_enum_headers
)

gvnc_enum_sources = gnome.mkenums(
  'vncconnectionenums.c',
  sources: ['vncconnection.h'],
  comments: '/* @comment@ */',
  fhead: '#include "vncconnectionenums.h"\n\n',
  fprod: '\n/* enumerations from "@filename@" */\n#include "@filename@"\n',
  vhead: 'static const G@Type@Value _@enum_name@_values[] = {',
  vprod: '  { @VALUENAME@, "@VALUENAME@", "@valuenick@" },',
  vtail: '  { 0, NULL, NULL }\n};\n\nGType\n@enum_name@_get_type (void)\n{\n  static GType type = 0;\n\n  if (!type)\n    type = g_@type@_register_static ("@EnumName@", _@enum_name@_values);\n\n  return type;\n}\n\n',
  install_header: false,
)

gvnc_enum_sources_dep = declare_dependency(
  sources: gvnc_enum_sources
)

gvnc_headers = [
  'gvnc.h',
  'vncpixelformat.h',
  'vncaudioformat.h',
  'vncaudiosample.h',
  'vncaudio.h',
  'vncbaseaudio.h',
  'vncframebuffer.h',
  'vncbaseframebuffer.h',
  'vnccursor.h',
  'vnccolormap.h',
  'vncconnection.h',
  'vncutil.h',
]

install_headers(gvnc_headers, subdir: 'gvnc-1.0')

gvnc_gir_sources = gvnc_headers + [
  gvnc_enum_headers,
  gvnc_enum_sources,
  'vncpixelformat.c',
  'vncaudioformat.c',
  'vncaudiosample.c',
  'vncaudio.c',
  'vncbaseaudio.c',
  'vncframebuffer.c',
  'vncbaseframebuffer.c',
  'vnccursor.c',
  'vnccolormap.c',
  'vncconnection.c',
  'vncutil.c',
]

gvnc_sources = gvnc_gir_sources + gvnc_marshal + [
  'coroutine.h',
  'dh.h',
  'dh.c',
  'vncbaseframebufferblt.h',
  'vncconnectionblt.h',
]

if coroutine == 'ucontext'
  gvnc_sources += [
         'coroutine_ucontext.c',
         'continuation.h',
         'continuation.c',
  ]
else
  gvnc_sources += 'coroutine_gthread.c'
endif

gvnc_link_args = []
if host_machine.system() != 'windows'
  gvnc_link_args += ['-Wl,--no-undefined']
endif

if host_machine.system() == 'linux'
  gvnc_link_args += ['-Wl,--version-script,@0@/@1@'.format(
    meson.current_source_dir(), 'libgvnc_sym.version')
]
endif

gvnc_deps = [
  gobject_dep,
  gio_dep,
  gdk_pixbuf_dep,
  gcrypt_dep,
  gnutls_dep,
  libsasl_dep,
  zlib_dep,
  gvnc_enum_headers_dep,
  gvnc_enum_sources_dep,
]

gvnc_inc = [
  src_incdir,
  top_incdir,
]

gvnc = library(
  'gvnc-1.0',
  sources: gvnc_sources,
  dependencies: gvnc_deps,
  include_directories: gvnc_inc,
  link_args: gvnc_link_args,
  version: '0.0.1',
  soversion: '0',
  install: true,
)

gvnc_dep = declare_dependency(
  sources: [gvnc_enum_headers],
  link_with: gvnc,
  dependencies: gvnc_deps,
  include_directories: gvnc_inc,
)

if host_machine.system() != 'windows'
  gvnc_gir = gnome.generate_gir(
    gvnc,
    sources: gvnc_gir_sources,
    nsversion: '1.0',
    namespace: 'GVnc',
    export_packages: 'gvnc-1.0',
    includes: ['GObject-2.0', 'Gio-2.0'],
    dependencies: gvnc_deps,
    header: 'gvnc.h',
    identifier_prefix: 'Vnc',
    symbol_prefix: 'vnc',
    install: true,
    install_dir_gir: gtk_vnc_girdir,
    install_dir_typelib: gtk_vnc_typelibdir,
  )

  gvnc_gir_dep = declare_dependency(
    sources: gvnc_gir
  )

  if with_vala
    gvnc_vapi = gnome.generate_vapi(
      'gvnc-1.0',
      install: true,
      packages: ['gobject-2.0', 'gio-2.0'],
      sources: gvnc_gir[0],
    )
  endif
endif

if libpulse_dep.found()
  gvncpulse_headers = [
    'gvncpulse.h',
    'vncaudiopulse.h',
  ]

  install_headers(gvncpulse_headers, subdir: 'gvncpulse-1.0')

  gvncpulse_sources = gvncpulse_headers + [
    'vncaudiopulse.c',
  ]

  gvncpulse_link_args = []
  if host_machine.system() != 'windows'
    gvncpulse_link_args += ['-Wl,--no-undefined']
  endif

  if host_machine.system() == 'linux'
    gvncpulse_link_args += ['-Wl,--version-script,@0@/@1@'.format(
      meson.current_source_dir(), 'libgvncpulse_sym.version')
  ]
  endif

  gvncpulse_deps = [
    gobject_dep,
    libpulse_dep,
    gvnc_dep,
  ]

  gvncpulse_inc = [
    src_incdir,
    top_incdir,
  ]

  gvncpulse = library(
    'gvncpulse-1.0',
    sources: gvncpulse_sources,
    dependencies: gvncpulse_deps,
    include_directories: gvncpulse_inc,
    link_args: gvncpulse_link_args,
    version: '0.0.1',
    soversion: '0',
    install: true,
  )

  gvncpulse_dep = declare_dependency(
    link_with: gvncpulse,
    dependencies: gvncpulse_deps,
    include_directories: gvncpulse_inc,
  )

  if host_machine.system() != 'windows'
    gvncpulse_gir = gnome.generate_gir(
      gvncpulse,
      sources: gvncpulse_sources,
      nsversion: '1.0',
      namespace: 'GVncPulse',
      export_packages: 'gvncpulse-1.0',
      includes: ['GObject-2.0', 'GVnc-1.0'],
      dependencies: gvncpulse_deps + [gvnc_gir_dep],
      header: 'gvncpulse.h',
      identifier_prefix: 'Vnc',
      symbol_prefix: 'vnc',
      install: true,
      install_dir_gir: gtk_vnc_girdir,
      install_dir_typelib: gtk_vnc_typelibdir,
    )

    gvncpulse_gir_dep = declare_dependency(
      sources: gvncpulse_gir
    )

    if with_vala
      gvncpulse_vapi = gnome.generate_vapi(
        'gvncpulse-1.0',
        install: true,
        packages: ['gobject-2.0', 'gio-2.0', gvnc_vapi],
        gir_dirs: meson.current_build_dir(),
        vapi_dirs: meson.current_build_dir(),
        sources: gvncpulse_gir[0],
      )
    endif
  endif
endif



gtk_vnc_enum_headers = gnome.mkenums(
  'vncdisplayenums.h',
  sources: ['vncdisplay.h'],
  comments: '/* @comment@ */',
  fhead: '#ifndef VNC_DISPLAY_ENUMS_H\n#define VNC_DISPLAY_ENUMS_H\n\n#include <glib-object.h>\nG_BEGIN_DECLS\n\n',
  ftail: 'G_END_DECLS\n\n#endif /* VNC_DISPLAY_ENUMS_H */\n',
  fprod: '\n/* --- vncdisplay.h --- */',
  eprod: '#define VNC_TYPE_@ENUMSHORT@ @enum_name@_get_type()\nGType @enum_name@_get_type (void);\n',
  install_header: true,
  install_dir: join_paths(gtk_vnc_includedir, 'gtk-vnc-2.0'),
)

gtk_vnc_enum_headers_dep = declare_dependency(
  sources: gtk_vnc_enum_headers
)

gtk_vnc_enum_sources = gnome.mkenums(
  'vncdisplayenums.c',
  sources: ['vncdisplay.h'],
  comments: '/* @comment@ */',
  fhead: '#include "vncdisplayenums.h"\n\n',
  fprod: '\n/* enumerations from "@filename@" */\n#include "@filename@"\n',
  vhead: 'static const G@Type@Value _@enum_name@_values[] = {',
  vprod: '  { @VALUENAME@, "@VALUENAME@", "@valuenick@" },',
  vtail: '  { 0, NULL, NULL }\n};\n\nGType\n@enum_name@_get_type (void)\n{\n  static GType type = 0;\n\n  if (!type)\n    type = g_@type@_register_static ("@EnumName@", _@enum_name@_values);\n\n  return type;\n}\n\n',
  install_header: false,
)

gtk_vnc_enum_sources_dep = declare_dependency(
  sources: gtk_vnc_enum_sources
)

gtk_vnc_headers = [
  'gtk-vnc.h',
  'vncdisplay.h',
  'vncgrabsequence.h',
  'vnccairoframebuffer.h',
]

install_headers(gtk_vnc_headers, subdir: 'gtk-vnc-2.0')

gtk_vnc_gir_sources = gtk_vnc_headers + [
  gtk_vnc_enum_headers,
  gtk_vnc_enum_sources,
  'vncdisplay.c',
  'vncgrabsequence.c',
  'vnccairoframebuffer.c',
]

gtk_vnc_sources = gtk_vnc_gir_sources + gvnc_marshal + [
  'vncdisplaykeymap.c',
  'vncdisplaykeymap.c',
]

python = import('python').find_installation()
keymaps = [
  'xorgevdev',
  'xorgkbd',
  'xorgxquartz',
  'xorgxwin',
  'osx',
  'win32',
  'x11'
]

foreach keymap: keymaps
  varname = 'keymap_@0@2qnum'.format(keymap)
  target = 'vncdisplay@0@.h'.format(varname)
  cmd = [
    python,
    keymapgen,
    '--lang', 'glib2',
    '--varname', varname,
    'code-map', keymapcsv, keymap, 'qnum'
  ]
  gtk_vnc_sources += custom_target(
    target,
    output: target,
    capture: true,
    build_by_default: true,
    command: cmd)
endforeach


gtk_vnc_link_args = []
if host_machine.system() != 'windows'
  gtk_vnc_link_args += ['-Wl,--no-undefined']
endif

if host_machine.system() == 'linux'
  gtk_vnc_link_args += ['-Wl,--version-script,@0@/@1@'.format(
    meson.current_source_dir(), 'libgtk-vnc_sym.version')
]
endif

gtk_vnc_deps = [
  gtk_dep,
  gvnc_dep,
  gtk_vnc_enum_headers_dep,
  gtk_vnc_enum_sources_dep,
]
if x11_dep.found()
  gtk_vnc_deps += x11_dep
endif

gtk_vnc_inc = [
  src_incdir,
  top_incdir,
]

gtk_vnc = library(
  'gtk-vnc-2.0',
  sources: gtk_vnc_sources,
  dependencies: gtk_vnc_deps,
  include_directories: gtk_vnc_inc,
  link_args: gtk_vnc_link_args,
  version: '0.0.2',
  soversion: '0',
  install: true,
)

gtk_vnc_dep = declare_dependency(
  link_with: gtk_vnc,
  dependencies: gtk_vnc_deps,
  include_directories: gtk_vnc_inc,
)

if host_machine.system() != 'windows'
  gtk_vnc_gir = gnome.generate_gir(
    gtk_vnc,
    sources: gtk_vnc_gir_sources,
    nsversion: '2.0',
    namespace: 'GtkVnc',
    export_packages: 'gtk-vnc-2.0',
    includes: ['GObject-2.0', 'Gtk-3.0', 'GVnc-1.0'],
    dependencies: gtk_vnc_deps + [gvnc_gir_dep],
    header: 'gtk-vnc.h',
    identifier_prefix: 'Vnc',
    symbol_prefix: 'vnc',
    install: true,
    install_dir_gir: gtk_vnc_girdir,
    install_dir_typelib: gtk_vnc_typelibdir,
  )

  gtk_vnc_gir_dep = declare_dependency(
    sources: gtk_vnc_gir
  )

  if with_vala
    gtk_vnc_vapi = gnome.generate_vapi(
      'gtk-vnc-2.0',
      install: true,
      packages: ['gobject-2.0', 'gio-2.0', 'gtk+-3.0', gvnc_vapi],
      gir_dirs: meson.current_build_dir(),
      vapi_dirs: meson.current_build_dir(),
      sources: gtk_vnc_gir[0],
    )
  endif
endif


vncconnectiontest = executable(
  'vncconnectiontest',
  sources: ['vncconnectiontest.c'],
  dependencies: [gvnc_dep]
)
test('vncconnectiontest', vncconnectiontest)
