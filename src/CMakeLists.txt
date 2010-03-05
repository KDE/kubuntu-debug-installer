set(installdbgsymbols_SRCS
   dbglookupthread.cpp
   dbginstaller.cpp
   main.cpp
 )

kde4_add_executable(installdbgsymbols ${installdbgsymbols_SRCS})

target_link_libraries(installdbgsymbols ${KDE4_KDEUI_LIBS} )

install(TARGETS installdbgsymbols ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( FILES installdbgsymbols.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR} )
