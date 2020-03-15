Notes:
Adding code in the middle of an ELF64 or changing the data lenght inside it is:
  - hard to do (**really** hard to make it work)
  - fucks up any debugging (RIP debug symbols, backtracing `0x... in ?? ()` sucks)
  - hard to implement for other formats (elf32, match-o, ...)

The solution:
- extract to anonymous file & run !
- still modify the elf file to encrypt it: (because we want to be stealth)
  - encrypt with AES in CFB mode (stream cipher) -> len(output) == len(input)
  - append new entrypoint to end of elf64 so nothing needs to be updated ;)

AES in CFB mode, which turns it into a stream cipher
