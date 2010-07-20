project(kubuntu-debug-installer)

find_package(KDE4 REQUIRED)
find_package(QApt REQUIRED)
include (KDE4Defaults)

macro_log_feature(QAPT_FOUND "QApt" "High-level Qt wrapper/Apt implementation around the Apt-pkg library"
                  "http://kubuntu.org" FALSE "" "Used to look up packages")

include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )

add_subdirectory( src )
