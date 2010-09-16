configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/Version.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(installdbgsymbols_SRCS
    DebugFinder.cpp
    DebugInstaller.cpp
    main.cpp
)

kde4_add_executable(kubuntu-debug-installer ${installdbgsymbols_SRCS})

target_link_libraries(kubuntu-debug-installer ${KDE4_KDEUI_LIBS} ${QAPT_LIBRARY})

install(TARGETS kubuntu-debug-installer ${INSTALL_TARGETS_DEFAULT_ARGS} )
