project(skog)

find_package(jsoncpp REQUIRED)
find_package(Boost COMPONENTS system program_options regex locale date_time filesystem REQUIRED)

include_directories(SYSTEM
                    ${Boost_INCLUDE_DIRS}
					${jsoncpp_INCLUDE_DIRS}
                    ${SuperMarx_INCLUDE_DIRS})
                    
add_library(skogcore scraper.cpp)
target_link_libraries(skogcore
					  supermarx-scraper
					  ${jsoncpp_LIBRARIES}
                      ${Boost_LIBRARIES}) 

add_executable(skog main.cpp)
target_link_libraries(skog
					  skogcore
					  supermarx-api)

include_directories(${skog_INCLUDE_DIRS})
