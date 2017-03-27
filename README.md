	

# tmmo project (Text-based MMORPG) [![Build Status](https://travis-ci.org/zuZuz/tmmo.svg?branch=master)](https://travis-ci.org/zuZuz/tmmo)

Project includes:

1. UDP Client ([Zxobi](https://github.com/Zxobi))
2. UDP Server ([zuZuz](https://github.com/ZuZuz))
3. Own simple database implementation ([ditrol](https://github.com/ditrol))
4. World Generator & AI & GamePlay ([prestoV](https://github.com/prestoV))

## client

TODO

## server

TODO

## map generator/AI/gameplay

TODO

## key-value database
### requirements

- sglib 1.0.4 
- libuv 1.5.0

### syntax

    <cmd><key>\x00[value]

### supported commands

- S: set value
- G: get value
- D: delete value
- X: shutdown

