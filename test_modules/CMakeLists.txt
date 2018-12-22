project(test_modules)

SET(APP_TEST_BASE "DEP_EMBTOM_INTREE")
SET(LIB_MENU_BASE "DEP_EMBTOM_INTREE")
SET(TEST_CASES_BASE "DEP_EMBTOM_INTREE")

add_subdirectory(app_test)
add_subdirectory(lib_menu)
add_subdirectory(lib_test_cases)
add_subdirectory(lib_test_framework_EMBUNIT)

