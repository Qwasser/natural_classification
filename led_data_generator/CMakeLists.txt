CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

PROJECT( led_data_generator )

file(GLOB_RECURSE LED_SIMPLE_SRC "led-creator.c")
add_executable(led_simple ${LED_SIMPLE_SRC})

file(GLOB_RECURSE LED_IRRELEVANT_SRC "led-creator-+17.c")
add_executable(led_irrelevant ${LED_IRRELEVANT_SRC})




