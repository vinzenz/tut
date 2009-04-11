FILE(GLOB basic_FILES       "basic/*.cpp")
FILE(GLOB restartable_FILES "restartable/*.cpp")
FILE(GLOB shared_ptr_FILES  "shared_ptr/*.cpp")

ADD_EXECUTABLE(ex_basic       ${basic_FILES})
ADD_EXECUTABLE(ex_restartable ${restartable_FILES})
ADD_EXECUTABLE(ex_shared_ptr  ${shared_ptr_FILES})

#FILE(GLOB plugins_FILES     "plugins/*.cpp")
FILE(GLOB reflection_FILES  "reflection/*.cpp")
FILE(GLOB functions_FILES   "functions/*.cpp")

#ADD_EXECUTABLE(plugins     ${plugins_FILES})
LINK_LIBRARIES(reflection rt)

ADD_EXECUTABLE(ex_reflection ${reflection_FILES})
set_property(TARGET ex_reflection PROPERTY COMPILE_FLAGS
             "-finstrument-functions -g")

ADD_EXECUTABLE(ex_functions ${functions_FILES})
set_property(TARGET ex_functions PROPERTY COMPILE_FLAGS
             "-finstrument-functions -g -DTUT_USE_FUNCTIONS")
