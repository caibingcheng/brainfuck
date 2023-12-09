### Brainfuck

My implemention for brainfuck.

### Usage

Compile:
```
g++ --std=c++11 ./brainfuck.cc -o brainfuck
```

And run `helloworld.bf` with debug mode:
```
./brainfuck helloworld.bf -d
```

More command params of brainfuck:
```
Usage: ./brainfuck [options]
Options:
  -s --stack <size> set stack size
  -f --file <path> set source file
  -c --cli use command line mode
  -h --help show help
```