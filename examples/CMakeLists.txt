FILE(GLOB basic_FILES       "basic/*.cpp")
FILE(GLOB restartable_FILES "restartable/*.cpp")
FILE(GLOB shared_ptr_FILES  "shared_ptr/*.cpp")

ADD_EXECUTABLE(ex_basic       ${basic_FILES})
ADD_EXECUTABLE(ex_restartable ${restartable_FILES})
ADD_EXECUTABLE(ex_shared_ptr  ${shared_ptr_FILES})

#FILE(GLOB plugins_FILES     "plugins/*.cpp")
FILE(GLOB reflect_FILES  "reflect/*.cpp")
FILE(GLOB functions_FILES   "functions/*.cpp")

#ADD_EXECUTABLE(plugins     ${plugins_FILES})
LINK_LIBRARIES(reflect rt)

ADD_EXECUTABLE(ex_reflect ${reflection_FILES})
set_property(TARGET ex_reflect PROPERTY COMPILE_FLAGS
             "-finstrument-functions -g")

ADD_EXECUTABLE(ex_functions ${functions_FILES})
set_property(TARGET ex_functions PROPERTY COMPILE_FLAGS
             "-finstrument-functions -g -DTUT_USE_FUNCTIONS")
