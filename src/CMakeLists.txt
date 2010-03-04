set(installdbgsymbols_SRCS
   dbginstaller.cpp
   main.cpp
 )

kde4_add_ui_files(installdbgsymbols_SRCS dbginstaller.ui)

kde4_add_executable(installdbgsymbols ${installdbgsymbols_SRCS})

target_link_libraries(installdbgsymbols ${KDE4_KDEUI_LIBS} )

install(TARGETS installdbgsymbols ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( FILES installdbgsymbols.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR} )
