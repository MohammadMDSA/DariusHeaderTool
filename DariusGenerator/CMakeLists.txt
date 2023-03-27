cmake_minimum_required(VERSION 3.13.5)

project(DariusCodeGenerator)

###########################################
# Configure the demo project file generator
###########################################

add_executable(DariusCodeGenerator
					CppPropsParser.cpp
					main.cpp)

target_compile_features(DariusCodeGenerator PUBLIC cxx_std_20)

target_include_directories(DariusCodeGenerator PRIVATE
							.)

target_link_libraries(DariusCodeGenerator PRIVATE Kodgen)
