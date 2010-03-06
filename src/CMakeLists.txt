set(installdbgsymbols_SRCS
   dbglookupthread.cpp
   dbginstaller.cpp
   main.cpp
 )

kde4_add_executable(kubuntu-debug-installer ${installdbgsymbols_SRCS})

target_link_libraries(kubuntu-debug-installer ${KDE4_KDEUI_LIBS} )

install(TARGETS kubuntu-debug-installer ${INSTALL_TARGETS_DEFAULT_ARGS} )
