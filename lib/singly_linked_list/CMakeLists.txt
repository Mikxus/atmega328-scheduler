
# Create singly linked list libary
add_avr_library(
   singly_linked_list
   singly_linked_list.h 
)

# fix: cmake not being able to determine linker language 
set_target_properties(singly_linked_list-${AVR_MCU} PROPERTIES LINKER_LANGUAGE CXX)
