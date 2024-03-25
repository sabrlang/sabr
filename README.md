# Sabr Programming Language

# How to run
## Compile to bytecode
```sh
$ sabr -c {source file name} -o {output file name}
```
## Run bytecode
```sh
$ sabr -e {bytecode file name}
```

# Specification
Sabr programs must be written in UTF-8.

## Data Types
Sabr has no type checking. All stack values are 8 bytes wide.

* `x` : Cell (any)
* `n` : Integer
* `s` : Signed integer
* `u` : Unsigned integer
* `f` : Float-point
* `b` : Boolean
* `addr` : Address
* `id` : Identifier

## Literals
### Number literals
* Integers : `255`, `0255`, `0xff`, `0o377`, `0b11111111`
* Floating-point : `0.25`, `.25`, `00.250`, `0.25e0`, `2.5e-1`, `0.025e1`

### Character literals
* Unicode characters : `'あ'` -> `12354`
* Characters sequence : `'Hello'` -> `111 108 108 101 72`

#### Escape sequences
* `\a` -> 7
* `\b` -> 8
* `\e` -> 27
* `\f` -> 12
* `\n` -> 10
* `\r` -> 13
* `\t` -> 9
* `\v` -> 11
* `\\` -> 92
* `\'` -> 39
* `\"` -> 34
* `\nnn` -> The byte whose numerical value is given by nnn interpreted as an octal number.
* `\xhh` -> The byte whose numerical value is given by hh interpreted as a hexadecimal number.
* `\uhhhh` -> Unicode code point below 10000 hexadecimal.
* `\Uhhhhhhhh` -> Unicode code point where h is a hexadecimal digit.

### String literals
* `"Hello"` -> It `allot` 6 cells. It ends with '\0'.

## Array
```
[ 1 , 2 , 3 , 4 , 5 ]
```
`[` begins declaring the array.

`,` stores value in the array.

`]` stores value in the array and terminates the array declaration.
It will `allot` 5 cells.

Multidimensional arrays are also possible.

```
[ [ 1 , 2 , 3 ] , [ 4 , 5 , 6 ] , [ 7 , 8 , 9 ] ]
```

## Identifier literals
```
$(identifier)
```
This is a unique unsigned integer value.

It used when declaring subroutines, variables, structures, etc.

Control keywords, built-in operators, literals cannot become identifiers.

## Memory
### Variables
* `set` ( x id -- ) \
	Store the value *x* in variables.
	*id* will be variable's name and it returns value of the variable.

	Variables declared globally are stored in global memory.
	And variables declared within a function remain within the function's stack.

* `addr` ( id -- addr ) \
	Returns address of the variable.

* `ref` ( addr id -- ) \
	Declares a reference to memory cell *addr*.
	References are used like variables.

### Memory access
* `fetch` ( addr -- x ) \
	Fetch the value that stored at *addr*.

* `store` ( x addr -- ) \
	Store the value *x* into the memory cell *addr*.

### Stack memory allocation
* `allot` ( u -- addr ) \
	Allocates *u* bytes from stack memory.
  
	If you allocate at global scope, it is allocated in global stack memory.
	If allocated within a function, it is allocated in the function stack memory.
  
	Memory blocks allocated with `allot` does not need to be deallocated.

### Dynamic memory allocation
* `alloc` ( u -- addr ) \
	Allocates *u* bytes from heap.
	Memory blocks allocated with `allot` must be freed.

* `free` ( addr - ) \
	Free memory block *addr*.

* `resize` ( u addr -- addr ) \
	Change the size of the memory block *addr* to *u* bytes.
	And return a pointer to the re-allocated memory.

## Controls
### Conditionals
#### if statements
False is 0, and true is non-zero values.

```
(flag) if
	(code)
end
```
`if` pops the flag value from the stack.  
If `(flag)` is true, `(code)` is executed.

```
(flag) if
	(code 1)
else
	(code 2)
end
```
If `(flag)` is true, `(code 1)` is executed.  
If `(flag)` is false, `(code 2)` is executed.

#### switch statements
```
(value) switch
	(case 1) case (code 1) pass
	(case 2) case (code 2) pass
	(case 3) case
	(case 4) case
		(code 3)
	pass
	(code 4)
end
```

`switch` and `case` pop the value from the stack.  
If `(value)` is equal to `(case 1)`, `(code 1)` is executed.  
If `(value)` is equal to `(case 3)` or `(case 4)`, `(code 3)` is executed.  
If there is no matching value, `(code 4)` is executed.

##### if-elif-else style
```
(non-zero) switch
	(flag 1) case (code 1) pass
	(flag 2) case (code 2) pass
	(flag 3) case
	(flag 4) case
		(code 3)
	pass
	(code 4)
end
```

If `(flag 1)` is true, `(code 1)` is executed.  
If `(flag 3)` or `(flag 4)` is true, `(code 3)` is excuted.  
If every flag is false, `(code 4)` is executed.

### Loops
#### loop statements
```
loop
	(code 1)
end
```

This is an endless loop.

```
loop
	(code)
	(flag)
	while
end
```

`(code)` is excuted. `while` pops the flag value from the stack.  
If `(flag)` is true, the loop is restarted.

```
loop
	(code 1)
	(flag)
	while
	(code 2)
end
```

`(code 1)` is executed. `while` pops the flag value from the stack.  
If `(flag)` is true, `(code 2)` is executed and the loop is restarted.

#### for statements
```
$(identifier) for (start) from (end) to (step) step
	(code)
end 
```

`for`, `from`, `to`, `step` pop the values from stack.  
`(identifier)` will be counter variable's name.

`(start)` is starting value.  
if `(start) from` is not present, `(start)` is treated as 0.

The loop ends when the value of the variable exceeds `(end)`.  
if `(end) to` is not present, the loop will be infinite loop.

Each iteration increments the value of the variable by `(step)`.  
if `(step) step` is not present, `(step)` is treated as 1.

`for` only works with signed integers.  
You can use `ufor` for unsigned integers and `ffor` for floating point.

#### continue and break
```
loop
	continue
end
```

`continue` skips the rest of the current iteration.

```
loop
	break
end
```

`break` terminates the loop.

## Subroutines
```
$(identifier) func
	(code 1)
	return
defer
	(code 2)
end
```
```
$(identifier) macro
	(code)
end
```
`function` has local variables and local memories. but `macro` is not.
`(identifier)` calls subroutines.

You can terminate a subroutine early with `return`.
`return` is not necessary.

`(code 2)` after `defer` is executed after `return`.
`defer` is not necessary.

## Structures
```
$(struct identifier) struct
	$(member identifier 1) member
	$(member identifier 2) member
	$(member identifier 3) member
	...
end
```
`struct` and `memeber` pop values from the stack.
`(struct identifier)` will be structure's name, and it returns size of structure.
`(struct identifier).(member identifier 1)` pops structure address value from the stack. And it returns member address.


### Example
A struct is declared like this:
```
$Pos struct
	$x member
	$y member
end
```
Pos is the name of the structure, and it has two member variables, x and y.

To use a structure, allocate memory space equal to the size of the structure. The name of the structure return the size of the structure.
The members of the structure are used in the form `Pos.x`, It must be preceded by the address of a block of memory in that structure type. This will return the address of the member variable.
```
Pos cells allot $p1 set \ After allocating the Pos structure, it is assigned to the p1 variable.
50 p1 Pos.x store \ Storing the value of 50 in the member variable x of p1.
p1 Pos.x fetch puti \ Outputs the value of the member variable x of p1.
```