# The bytecode

## Basic format of a Kafe bytecode file

* header
    * magic constant "kafe" on 4 bytes
    * VM version on 3 bytes: 1 for MAJOR, 1 for MINOR, 1 for PATCH
    * 1 byte of padding (should be null)
    * timestamp (unix format) on 4 bytes (we should consider using 8 bytes)
    * MD5 hash on 512 bytes
* segments
    * contants table
        * number of constants on 2 bytes
        * constants
            * type on 1 byte
            * value encoded regarging its type
    * symbols table
        * number of symbols on 2 bytes
        * symbols
            * symbol name null-terminated
    * classes table
        * number of classes on 2 bytes
        * classes
            * number of attributes on 2 bytes
            * class name (should be a symbol index) on 2 bytes
            * attributes
                * name (should be a symbol index) on 2 bytes
                * value (should be a constant index) on 2 bytes
    * code segment(s)
        * number of opcodes on 2 bytes
        * opcodes
            * op code on 1 byte
            * argument(s) on 2 bytes each