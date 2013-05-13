ADD_LIBRARY(scl STATIC socketmap.cpp sclhelper.c)
ADD_LIBRARY(scl_shared SHARED socketmap.cpp sclhelper.c)

TARGET_LINK_LIBRARIES(scl zmq)
TARGET_LINK_LIBRARIES(scl_shared zmq yaml-cpp)
