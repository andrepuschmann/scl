INCLUDE(ExternalProject)
ExternalProject_Add(lib-yaml-cpp
   DOWNLOAD_COMMAND
   URL https://yaml-cpp.googlecode.com/files/yaml-cpp-0.2.7.tar.gz
   URL_MD5 6878e14bad90c69a8f2caca273eb24c2
)
