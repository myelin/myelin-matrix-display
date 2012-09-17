Python client
=============

matrix.py is a Python library to drive the matrix display over UDP:

    import matrix
    matrix.show(matrix.fill(matrix.random_color())) # set all pixels to the same random colour

    f = matrix.Frame()
    f.box(1, 1, f.width - 2, f.height - 2, random_color())
    f.line(1, 1, f.width - 2, f.height - 2, (255, 255, 255))
    f.line(1, f.height - 2, f.width - 2, 1, (255, 255, 255))
    f.show()

28_show.py and 42_show.py are Python
scripts, courtesy [Philip Lindsay](http://rancidbacon.com/), that
animate the 25x12 display.