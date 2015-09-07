# find ViennaCL
# picks up the VIENNACLPATH environment variable for detecting ViennaCL
find_path(ViennaCL_INCLUDE_DIR 
          NAMES viennacl/vector.hpp
          PATHS "$ENV{VIENNACLPATH}")
if(NOT ViennaCL_INCLUDE_DIR)
  message(SEND_ERROR "Failed to find ViennaCL")
endif()
mark_as_advanced(ViennaCL_INCLUDE_DIR)

include_directories(${ViennaCL_INCLUDE_DIRS} "${ViennaCL_INCLUDE_DIR}")
