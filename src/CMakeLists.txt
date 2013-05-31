ADD_LIBRARY(scl STATIC socketmap.cpp sclhelper.c)
ADD_LIBRARY(scl_shared SHARED socketmap.cpp sclhelper.c)

INCLUDE_DIRECTORIES(${YAML_CPP_SOURCE_DIR}/include)

TARGET_LINK_LIBRARIES(scl zmq)
TARGET_LINK_LIBRARIES(scl_shared zmq yaml-cpp)

INSTALL(TARGETS scl
    LIBRARY DESTINATION ${LIBRARY_DIR} # .so file
    ARCHIVE DESTINATION ${LIBRARY_DIR} # .lib file
    RUNTIME DESTINATION ${RUNTIME_DIR} # .dll file
)

INSTALL(TARGETS scl_shared
    LIBRARY DESTINATION ${LIBRARY_DIR} # .so file
    ARCHIVE DESTINATION ${LIBRARY_DIR} # .lib file
    RUNTIME DESTINATION ${RUNTIME_DIR} # .dll file
)

INSTALL(CODE
    "execute_process(COMMAND python setup.py install -f --prefix=${CMAKE_INSTALL_PREFIX} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})")
