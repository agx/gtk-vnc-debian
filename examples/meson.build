
gvncviewer_deps = [gtk_vnc_dep]

if libpulse_dep.found()
  gvncviewer_deps += [gvncpulse_dep]
endif

gvncviewer = executable(
  'gvncviewer',
  sources: ['gvncviewer.c'],
  dependencies: gvncviewer_deps,
  install: false,
)
