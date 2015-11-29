basic world viewer

This code was written by Derrall Heath for a CS 750 project. It should provide the graphical/visual portion of your TIP project.

USAGE: 
   `make run`
or
   `bin/world_viewer`

Use the arrrow keys to move/turn and pageup and pagedown to tilt the view.

It reads in a file (data/world.txt) from which it renders the scene. The world.txt has the following format:


NUMPOLLIES n

SHAPE 4
x1 y1 z1 u1 v1
x2 y2 z2 u2 v2
x3 y3 z3 u3 v3
x4 y4 z4 u4 v4
some_image.jpg

SHAPE 4
x1 y1 z1 u1 v1
x2 y2 z2 u2 v2
x3 y3 z3 u3 v3
x4 y4 z4 u4 v4
some_image.jpg
.
.
.


Where n is the number of rectangles you'll have in the scene. There as a word.txt file already written as an example. The images are expected to be in the data/ directory.

For TIP, you essentailly will have your code generate the images and world.txt file this program will take.

Feel free to modify the code. I don't know OpenGL very well, but I'm doing stuff in WebGL right now (which is very similar), so I may be able to help if you run into problems.

-Brian Davis
