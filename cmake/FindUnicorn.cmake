# Exports:
#  LIBUNICORN_FOUND
#  LIBUNICORN_INCLUDE_DIR
#  LIBUNICORN_LIBRARY

find_path(LIBUNICORN_INCLUDE_DIR
          unicorn.h
          HINTS $ENV{UNICORNDIR}
          PATH_SUFFIXES include/unicorn include)

find_library(LIBUNICORN_LIBRARY
             NAMES unicorn
             HINTS $ENV{UNICORNDIR})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(unicorn
                                  REQUIRED_VARS LIBUNICORN_LIBRARY LIBUNICORN_INCLUDE_DIR)
