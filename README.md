# text-file
Perform simple oprations on text taken from a file.
## Usage
`input`
```
a a a
b
c
d d

e
```
### Get file info
`./text-file input -c`
```
Char count: 23
Line count: 7
Word count: 14
Unique word count: 7
```
### Get word frequency and percent
`./text-file input -cs`
```
a 1 7.142858
b 1 7.142858
c 1 7.142858
d 1 7.142858
e 1 7.142858
f 1 7.142858
h 8 57.142860
```
### Search file
`./text-file input -s`
```
Query: e
3: e
Result count: 1
```
### Replace text
`./text-file input -r`
```
Query: a
To: a eee c
a eee c b c
d
e
f
  h
h
h h h h h h
Replace count: 1
```
