project(kubuntu-debug-installer)

find_package(KDE4 REQUIRED)
find_package(QApt REQUIRED)
include (KDE4Defaults)

macro_log_feature(QAPT_FOUND "QApt" "High-level Qt wrapper/Apt implementation around the Apt-pkg library"
                  "http://kubuntu.org" FALSE "" "Used to look up packages")

include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )

execute_process(COMMAND src/version.sh
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                RESULT_VARIABLE VERSION_RESULT
                OUTPUT_VARIABLE VERSION_STRING
                OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(NOT VERSION_RESULT EQUAL 0 OR VERSION_STRING STREQUAL "")
    message(FATAL_ERROR "Could not get version from debian/changelog :'-(")
endif(NOT VERSION_RESULT EQUAL 0 OR VERSION_STRING STREQUAL "")

add_subdirectory( src )
