cmake_minimum_required(VERSION 3.10)

project(main)
find_package(OpenMP REQUIRED)
add_executable(main
        src/main.cpp
        src/Vector.cpp
        src/Sphere.cpp
        src/Object.cpp
        src/Color.cpp
        src/Material.cpp
        src/Plane.cpp
        src/MirrorSphere.cpp
        src/Cylinder.cpp
        src/Triangle.cpp
        src/Mesh.cpp
        src/Matrix4.cpp
      )
target_include_directories(main PRIVATE include)


if(OpenMP_CXX_FOUND)
    target_link_libraries(main PUBLIC OpenMP::OpenMP_CXX)
endif()
