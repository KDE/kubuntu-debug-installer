project(kubuntu-debug-installer)

cmake_minimum_required(VERSION 2.8.12)

find_package(ECM 0.0.14 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)

find_package(KF5CoreAddons 5.0.0 REQUIRED)
find_package(KF5I18n 5.0.0 REQUIRED)
find_package(KF5WidgetsAddons 5.0.0 REQUIRED)

find_package(QApt 3.0.0 REQUIRED)

add_subdirectory(src)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
