if(POLICY CMP0028)
  cmake_policy(SET CMP0028 OLD)
endif()
remove_definitions(-DQT_NO_CAST_FROM_ASCII)

include(ECMAddTests)
include(ECMCoverageOption)

########### unittests ###############

ecm_add_test(../src/DebugFinder.cpp
             findertest.cpp
    NAME_PREFIX "kubuntu-debug-installer-"
    TEST_NAME "findertest"
    LINK_LIBRARIES Qt5::Test QApt::Main
)

# target_link_libraries(threadtest Qt5::Concurrent)


