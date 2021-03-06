# Copyright (c) 2017, Tristan Brindle, Tom Honermann
#
# This file is distributed under the MIT License. See the accompanying file
# LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
# and conditions.

cmake_minimum_required(VERSION 3.0.2)

project(text_view_test CXX)

add_executable(
  test-caching-iterator
  test-caching-iterator.cpp)
target_link_libraries(
  test-caching-iterator
  PRIVATE text-view)

include(CTest)
add_test(
  NAME test-caching-iterator
  COMMAND test-caching-iterator)

add_executable(
  test-encodings
  test-encodings.cpp)
target_link_libraries(
  test-encodings
  PRIVATE text-view)

include(CTest)
add_test(
  NAME test-encodings
  COMMAND test-encodings)

add_executable(
  test-error-handling
  test-error-handling.cpp)
target_link_libraries(
  test-error-handling
  PRIVATE text-view)

include(CTest)
add_test(
  NAME test-error-handling
  COMMAND test-error-handling)

add_executable(
  test-models
  test-models.cpp)
target_link_libraries(
  test-models
  PRIVATE text-view)

include(CTest)
add_test(
  NAME test-models
  COMMAND test-models)

add_executable(
  test-subobject
  test-subobject.cpp)
target_link_libraries(
  test-subobject
  PRIVATE text-view)

include(CTest)
add_test(
  NAME test-subobject
  COMMAND test-subobject)
