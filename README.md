# Analytical Placer #

## Build Instructions ##
To build the environment for the placer, start a terminal process and execute the following:

```bash
$ chmod +x build.sh
$ ./build.sh
```

## Compilation ##

To compile the analytical placer, begin executing a terminal and proceed with the following commands:

```bash
$ make
```

This should produce no errors, and minimal warnings.

If there are any errors during execution, please attempt the following:

```bash
$ make clean
$ make
```

## Usage ##

You can begin executing the placer by executing a terminal, and entering:

```bash
$ ./placer -file [filename]
```

1. There are several options for test circuits, such as `cct1`, `cct2`, `cct3`, `cct4`, as well as `oct1`, `oct2`, `oct3`, `oct4`;
2. A GUI should proceed to open, and execute. You can step through the progress of the placer by clicking the `Step >>` button. 
3. You can toggle the clique model visualization by clicking the `Nets [1 | 0]` button. `1` indicates on. `0` indicates off.
4. You can toggle the Block numbering by clicking the `Show #[1 | 0]` button. `1` indicates on. `0` indicates off.

And voila!

## Author ##

Nicholas V. Giamblanco, 2017
