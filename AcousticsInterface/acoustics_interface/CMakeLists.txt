cmake_minimum_required(VERSION 3.8)
project(acoustics_interface)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)

include_directories(include)

add_executable(acoustics_interface_node
  src/acoustics_interface_driver.c
  src/acoustics_interface_node.cpp
)

ament_target_dependencies(acoustics_interface_node
  rclcpp
  std_msgs
  spdlog
  fmt
)


target_link_libraries(acoustics_interface_node fmt::fmt)

install(TARGETS
acoustics_interface_node
DESTINATION lib/${PROJECT_NAME}
)



install(
  DIRECTORY include/
  DESTINATION include
)

install(DIRECTORY
  launch
  config
  DESTINATION share/${PROJECT_NAME}/
)

ament_package()
