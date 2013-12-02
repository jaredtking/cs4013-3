cs4013 [![Build Status](https://travis-ci.org/jaredtking/cs4013-2.png?branch=master)](https://travis-ci.org/jaredtking/cs4013-2)
======

Third compilers project for CS4013 at the University of Tulsa

### Requirements

- make
- gcc
- [Check](http://check.sourceforge.net/) (only for testing)

### Running

Compile the program with `make`.

```bash
make
```

Output from gcc will be in the `bin` directory.

```bash
./bin/parser reserved_words_file source_file output_dir
```

### Testing

Testing is provided with [Check](http://check.sourceforge.net/). Make sure it is installed before running tests.

```bash
make check
```

### License

Released under MIT license.
