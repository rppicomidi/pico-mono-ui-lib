# pico-mono-ui-lib

This library is intended to be used as a git submodule. It
implements in C++ a simple UI intended to run on a 128x64 dot OLED.
It has been developed to work with a lower-level graphics library
that provides drawing support and font support. It has been
developed to work with the C++ code in the
pico-ssd1306-mono-graphics-lib project. It is designed to run single
threaded within the Pico C SDK.

The basic viewable class is called a View. A view generally takes
up the whole screen. The View can contain anything that the
graphics library can draw. The UI is made up of multiple View
objects. The View_manager class maintains a stack of View
objects. The top of the View_manager stack is visible. The first
View added to the stack is considered the Home screen.

You can navigate the UI with a simple 7-button navigation
system. This libray supports a Nav_buttons class that
supports buttons Up, Down, Left, Right, Select, Shift, and Back.
All button press events go to the View_manager. The View_manager
routes all navigation button presses except Back and Shift+Back
to the View at the top of the View_manager stack. The Back
button will pop the top of the View_manager stack and show the
next View in the stack. The Shift+Back button combination will
pop all View objects from the View_manager stack except the home
screen.

Note that Up/Down and/or Left/Right events could be provided
by rotary encoders. Such drivers would just have to call
the appropriate functions in the View_manager to signal a new
event.

Most of the UI will be composed of drawn text and Menu class
objects. A Menu class object is a scrollable text menu that
shows a vertical progress bar to show what portion of the
text menu is visible on the display. Each line in a Menu is a
Menu_item object. There are several Menu_item classes that
support choosing an item from a list, pushing a new View object
to the View_manager stack, adjusting a single number, or
adjusting a pair of numbers.

Menu_items that adjust values operating on Setting_* class
objects indirectly. Setting_* class objects are contain range
checked values with default values. They support get, set,
increment, decrement, serialize to JSON and deserialize from JSON.
At the moment, setting types supported are numbers, 
an array of number pairs (for mapping one value to another),
and a selection from a list of strings (a poor-man's enum). Regular
strings and boolean values can be supported in the future if
required.