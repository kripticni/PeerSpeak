cmake_minimum_required(VERSION 3.16)
project(PeerSpeak VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL3 REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GPGME REQUIRED IMPORTED_TARGET gpgme)
find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(SQLiteCpp REQUIRED)

include(FetchContent)

FetchContent_Declare(
  asio
  GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
  GIT_TAG asio-1-34-2
)
FetchContent_MakeAvailable(asio)

FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG docking
)
FetchContent_MakeAvailable(imgui)

add_library(imgui STATIC
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_demo.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
  ${imgui_SOURCE_DIR}
  ${SDL3_INCLUDE_DIRS}
)

add_executable(PeerSpeak
  src/main.cpp
  src/User.cpp
  ${IMGUI_BACKEND_SOURCES}
)

target_include_directories(PeerSpeak PRIVATE
  include
  ${SDL3_INCLUDE_DIRS}
  ${asio_SOURCE_DIR}/asio/include
  ${imgui_SOURCE_DIR}
  ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(PeerSpeak PRIVATE
  imgui
  SDL3::SDL3
  OpenSSL::SSL
  OpenSSL::Crypto
  PkgConfig::GPGME
  OpenGL::OpenGL
  Threads::Threads
  SQLite::SQLite3
  ${SQLiteCpp_INCLUDE_DIRS}
)

target_compile_definitions(PeerSpeak PRIVATE
  ASIO_HAS_STD_COROUTINE
  ASIO_HAS_CO_AWAIT
)

if(ANDROID OR IOS)
  target_compile_definitions(PeerSpeak PRIVATE IMGUI_IMPL_OPENGL_ES3)
endif()

if(UNIX AND NOT APPLE)
  find_package(X11 REQUIRED)
  target_link_libraries(PeerSpeak PRIVATE X11::X11)
endif()

if(NIXOS)
    find_library(SSL_LIB ssl PATHS ${OPENSSL_LIBRARIES})
    find_library(CRYPTO_LIB crypto PATHS ${OPENSSL_LIBRARIES})
    target_link_libraries(echo_client_ssl PRIVATE ${SSL_LIB} ${CRYPTO_LIB})
endif()

add_executable(echo_server src/echo_server.cpp)
target_link_libraries(echo_server PRIVATE Threads::Threads)
target_include_directories(echo_server PRIVATE ${asio_SOURCE_DIR}/asio/include)

add_executable(echo_client src/echo_client.cpp)
target_link_libraries(echo_client PRIVATE Threads::Threads)
target_include_directories(echo_client PRIVATE ${asio_SOURCE_DIR}/asio/include)

add_executable(echo_client_tls src/echo_client_tls.cpp)
target_link_libraries(echo_client_tls PRIVATE 
  Threads::Threads
  OpenSSL::SSL
  OpenSSL::Crypto
)
target_include_directories(echo_client_tls PRIVATE 
  ${asio_SOURCE_DIR}/asio/include
)

add_executable(echo_server_tls src/echo_server_tls.cpp)
target_link_libraries(echo_server_tls PRIVATE 
  Threads::Threads
  OpenSSL::SSL
  OpenSSL::Crypto
)
target_include_directories(echo_server_tls PRIVATE 
  ${asio_SOURCE_DIR}/asio/include
)
