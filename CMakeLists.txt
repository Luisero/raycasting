cmake_minimum_required(VERSION 3.10)

project(main)
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
