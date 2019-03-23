# Kafe lang

## Basic rules

* every variable must have a type
* only const variables can be defined outside a class/method/function
* only first level class and functions can exist (can not create a function in a function, a class in a class or in a function)

## Creating variables and constants

*For this example, the rules given above aren't respected*

```
x : int  // this is a declaration
x2 : string = "hello"  // this is a definition
cst x3 : bool = false  // this is a constant
```

## Function calls

Function calls as instructions aren't accepted, only as expressions:

```
foo(1)  // won't work, ParseError
x: int = foo(1)  // no problem
```