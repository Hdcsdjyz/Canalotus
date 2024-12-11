cmake_minimum_required(VERSION 3.30)
project(Canalotus C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_FLAGS "-c -nostdlib")

add_executable(Canalotus src/kernel/main.c)
