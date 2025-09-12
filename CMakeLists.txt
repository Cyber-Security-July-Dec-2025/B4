
cmake_minimum_required(VERSION 3.16)
project(SecureChat LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable automoc/autouic so Qt meta-object code and ui files are handled automatically.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find Qt (Widgets + Network)
find_package(Qt6 REQUIRED COMPONENTS Widgets Network)

# Locate Crypto++ (try environment / brew prefix)
if(NOT DEFINED CRYPTOPP_ROOT)
  execute_process(COMMAND brew --prefix cryptopp
                  OUTPUT_VARIABLE CRYPTOPP_ROOT
                  OUTPUT_STRIP_TRAILING_WHITESPACE
                  ERROR_QUIET)
endif()

if(NOT CRYPTOPP_ROOT)
  message(WARNING "CRYPTOPP_ROOT not found via brew --prefix cryptopp. You can set -DCRYPTOPP_ROOT=/path/to/cryptopp when running cmake.")
endif()

# Prepare include and library paths for Crypto++
if(DEFINED CRYPTOPP_ROOT AND CRYPTOPP_ROOT)
  set(CRYPTOPP_INCLUDE_DIRS "${CRYPTOPP_ROOT}/include")
  set(CRYPTOPP_LIB_DIR "${CRYPTOPP_ROOT}/lib")
else()
  set(CRYPTOPP_INCLUDE_DIRS "")
  set(CRYPTOPP_LIB_DIR "")
endif()

# Try to find cryptopp library explicitly (libcryptopp.dylib / libcryptopp.a)
find_library(CRYPTOPP_LIBRARY
  NAMES cryptopp crypto++
  PATHS ${CRYPTOPP_LIB_DIR} /usr/local/lib /opt/homebrew/lib /usr/lib
  NO_DEFAULT_PATH
)

# Fallback: allow system to locate cryptopp if brew path wasn't provided
if(NOT CRYPTOPP_LIBRARY)
  find_library(CRYPTOPP_LIBRARY cryptopp)
endif()

if(NOT CRYPTOPP_LIBRARY)
  message(WARNING "Crypto++ lib not found automatically. You can pass -DCRYPTOPP_ROOT=$(brew --prefix cryptopp) to cmake or set CRYPTOPP_LIBRARY manually.")
else()
  message(STATUS "Found Crypto++ library: ${CRYPTOPP_LIBRARY}")
endif()

if(CRYPTOPP_INCLUDE_DIRS)
  include_directories(${CRYPTOPP_INCLUDE_DIRS})
endif()

if(CRYPTOPP_LIB_DIR)
  link_directories(${CRYPTOPP_LIB_DIR})
endif()

# Source files
set(SOURCES
  src/main.cpp
  src/MainWindow.cpp
  src/CryptoHelper.cpp
)

# Headers (optional to list)
set(HEADERS
  src/MainWindow.h
  src/CryptoHelper.h
)

add_executable(securechat ${SOURCES} ${HEADERS})

target_include_directories(securechat PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Link Qt modules and Crypto++ library
if(TARGET Qt6::Widgets AND TARGET Qt6::Network)
  target_link_libraries(securechat PRIVATE Qt6::Widgets Qt6::Network)
else()
  message(FATAL_ERROR "Qt6 Widgets/Network not found. Make sure CMAKE_PREFIX_PATH points to your Qt install (e.g. -DCMAKE_PREFIX_PATH=$(brew --prefix qt)/lib/cmake).")
endif()

if(CRYPTOPP_LIBRARY)
  target_link_libraries(securechat PRIVATE ${CRYPTOPP_LIBRARY})
else()
  message(WARNING "Proceeding without linking Crypto++ library; build will fail if Crypto++ symbols are needed.")
endif()
