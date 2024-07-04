

cmake_minimum_required(VERSION 3.0)

project(CALC)

# 输出一般日志信息
message(STATUS "status: ${PROJECT_SOURCE_DIR}")
# 输出警告信息
message(WARNING "warning: ${PROJECT_SOURCE_DIR}")

set(TEMP "Hello")
set(TEMP ${TEMP} " World")
message(${TEMP})

# 定义一个列表
set(myList "item1" "item2" "item3")

# APPEND
list(APPEND myList "item4")
message(STATUS "After APPEND: ${myList}")

# INSERT
list(INSERT myList 1 "newItem")
message(STATUS "After INSERT: ${myList}")

# PREPEND
list(PREPEND myList "newStartItem")
message(STATUS "After PREPEND: ${myList}")

# REMOVE_AT
list(REMOVE_AT myList 2)
message(STATUS "After REMOVE_AT: ${myList}")

# REMOVE_ITEM
list(REMOVE_ITEM myList "item3")
message(STATUS "After REMOVE_ITEM: ${myList}")

# REMOVE_DUPLICATES
list(APPEND myList "item4")
list(REMOVE_DUPLICATES myList)
message(STATUS "After REMOVE_DUPLICATES: ${myList}")

# REVERSE
list(REVERSE myList)
message(STATUS "After REVERSE: ${myList}")

# SORT
list(SORT myList)
message(STATUS "After SORT: ${myList}")

# LENGTH
list(LENGTH myList myListLength)
message(STATUS "List length: ${myListLength}")

# GET
list(GET myList 0 firstElement)
message(STATUS "First element: ${firstElement}")

# FIND
list(FIND myList "item2" index)
message(STATUS "Index of item2: ${index}")

# SUBLIST
list(SUBLIST myList 1 2 subList)
message(STATUS "Sublist: ${subList}")

# TRANSFORM
list(TRANSFORM myList TOUPPER)
message(STATUS "After TRANSFORM TOUPPER: ${myList}")

