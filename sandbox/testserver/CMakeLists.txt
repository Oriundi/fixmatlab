#*****************************************************************
# Copyright (C) 2010 by Accent Rouge bv                          *
#                                                                *
# This is proprietary software. No part of this software may be  *
# used, copied, distributed, compiled, sold, or modified in any  *
# form, or by any means, without permission from Accent Rouge bv *
#*****************************************************************
project( executor )
cmake_minimum_required( VERSION 2.4 )

find_package( Boost 1.38 COMPONENTS system thread date_time REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )

add_executable( executor executor.cpp application.cpp)
target_link_libraries(executor quickfix ${Boost_LIBRARIES})

