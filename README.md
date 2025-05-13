# submarine

Server in C.

## To communicate with the controller

1. ``make clean all``

2. ``./controller``

It is possible to use `help` in the *repl*, to get a list of functions.

3. In another *shell*, you can do ``nc localhost 5555``

To change the port, just modify the file `./controller.example.cfg`.

Then, you can do `./controller controller.example.cfg`.

### Prerequisities

You will need:

- `cmake`.

- `nc` alias *netcat* to test the *controller*.

- `clang` if you want to format the code.

## For the tests

1. ``clear && make clean test``

### EOF

