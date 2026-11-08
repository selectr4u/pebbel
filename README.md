# pebbel

a 64-bit virtual stack-based machine

this was a little educational project just to explore virtual machines and more specifically stack based machines. i had fun making it!

it is _very_ simple. if i was to re-iterate i would definitely consider items on the stack to be richer, and make use of unions for different types.

## operations available

| operation  | byte value                    | argument(s) required on stack | notes                                                                               |
| ---------- | ----------------------------- | ----------------------------- | ----------------------------------------------------------------------------------- |
| `push x`   | `1` + 8 bytes for number      | none                          | none                                                                                |
| `pop`      | `2`                           | none                          | none                                                                                |
| `add`      | `3`                           | `2` numbers on stack          | consumes the arguments and pushes result on stack                                   |
| `negate`   | `4`                           | `2` numbers on stack          | consumes the arguments and pushes result on stack                                   |
| `multiply` | `5`                           | `2` numbers on stack          | consumes the arguments and pushes result on stack                                   |
| `divide`   | `6`                           | `2` numbers on stack          | consumes the arguments and pushes result on stack. result's decimal is non-existent |
| `print`    | `7`                           | `1` number on stack           | outputs in form `[OUTPUT] x`                                                        |
| `goto x`   | `8` + 1 byte for label        | none                          | labels can be before or after                                                       |
| `label x`  | `9` + 1 byte for label        | none                          | a label must be a defined                                                           |
| `clone x`  | `10` + 1 byte for stack index | none                          | stack index must be <= to stack top and also >= 0.                                  |
| `halt`     | `11`                          | none                          | stops the program                                                                   |

## notes

- only 64 bit signed integers (everything is a signed integer)
- max stack size is 255 slots (arbitrary)
- max bytecode size is 2048 bytes (arbitrary)
- no functions or `store`/`load` operations

## usage

to use, pipe in raw bytecode into the program. for example `printf '\x02\x00\x00\x00\x00\x00\x00\x00\x0B\x07' | out/main` to `push 11`, `print` and then `halt`.
