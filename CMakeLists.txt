cmake_minimum_required (VERSION 3.19)

project (glsl-preprocessor-validator)

# define CMAKE_INSTALL_<dir> values
include(GNUInstallDirs)

set(header_files
	)

set(source_files
	"src/main.cpp"
	)

add_executable(glsl-preprocessor-validator ${source_files})

find_package(cxxopts CONFIG REQUIRED)

find_package(SPIRV-Tools-opt CONFIG REQUIRED)
find_package(glslang CONFIG REQUIRED)
find_package(unofficial-shaderc_util CONFIG REQUIRED)

find_package(unofficial-shaderc CONFIG REQUIRED)

# Remove the INTERFACE_LINK_LIBRARIES of shaderc because they cause a link error because of a wrong link_path and they are not useful
if(WIN32)
	set_target_properties(unofficial::shaderc::shaderc PROPERTIES
	  INTERFACE_LINK_LIBRARIES ""
	)
endif()

target_link_libraries(glsl-preprocessor-validator
	PRIVATE 
		cxxopts::cxxopts
		unofficial::shaderc_util::shaderc_util
		unofficial::shaderc::shaderc
	)

target_compile_features(glsl-preprocessor-validator
	PRIVATE 
		cxx_std_20
	)
	
install(
	TARGETS glsl-preprocessor-validator
    EXPORT glsl-preprocessor-validator-targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	)
	
install(
    EXPORT glsl-preprocessor-validator-targets 
    NAMESPACE glsl-preprocessor-validator::
    DESTINATION lib/cmake/glsl-preprocessor-validator
    )
	
install(FILES ${header_files} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES $<TARGET_FILE_DIR:glsl-preprocessor-validator>/glsl-preprocessor-validator.pdb DESTINATION ${CMAKE_INSTALL_LIBDIR} OPTIONAL)

add_subdirectory(test)