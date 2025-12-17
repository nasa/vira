add_executable(vira_moon vira_moon.cpp)
add_executable(vira_moon2 vira_moon2.cpp)
target_link_libraries(vira_moon PRIVATE vira)
target_link_libraries(vira_moon2 PRIVATE vira)