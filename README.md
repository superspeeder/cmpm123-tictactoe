# Tic Tac Toe

## Design Process

For this assignment, I have largely just followed the provided TODOs in the code and used the existing framework to implement this game.
I did add a helper function of my own to reduce code duplication and put the related board check logics into the same place, but otherwise I have not added much of my own design ideas to this program (the TODOs were detailed enough to not have much leeway to go on). Whenever I did need to do design things, my main goals are limiting heap allocations, reducing duplication, and trying to not iterate over things several times in a row when I can compress the logic into one iteration.

I also added my Logger class from the last assignment (and used it in a few places for debugging).

I also adjusted the `main_macos.cpp` file in an attempt to get the code working on my laptop (which runs Ubuntu). This is why I have added `glad` to the code. I wasn't able to test this though since I apparently broke opengl on my laptop earlier today and don't have time to fix it.

Also, my monitor has a high enough resolution that the builtin font to ImGui is practically unreadable as a result, so I have added a font which is easier to read/generate reasonable sized glyphs from. This isn't important to the assignment, moreso this was important to my being able to actually finish the assignment.


## Platform Used

This code has been tested on Windows.
