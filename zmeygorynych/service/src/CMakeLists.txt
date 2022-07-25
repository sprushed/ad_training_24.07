cmake_minimum_required(VERSION 3.18)
project(src)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-fno-stack-protector -O0)
add_executable(src main.cpp Database.cpp Database.h User.cpp User.h UserManager.cpp UserManager.h util.h util.cpp Entity.h UI.cpp UI.h asciiFont.h UILogin.cpp UIMain.cpp Entity.cpp Shop.cpp Shop.h Product.cpp Product.h consts.h UIShop.cpp)
target_link_libraries(src pqxx pq fmt readline)
