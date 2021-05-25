SYSC 4001A Assignment 1

Chase Badalato 
101072570

*************
Compiling
*************
1) untar files
2) in the directory run "make" to compile the source code
3) type "./Matric_Multiplier" to run
4) If you wish to run extra tests they are commented out at the bottom
   Simply comment the current test and copy paste the alternate tests right below the 
   location of the newly commented matrices

************
Discussion
************
The goal of this program is to efficiently calculate the resulting matrix Q when multiplying
matrices M and N.  

The very first thing that I needed to decide was if the use of semaphores was required. When I thought
about it I realized although all 4 chilren are entering shared memory but NONE of the children ever access the 
SAME location in shared memory.  Each child only has to deal with its specified row so it is not possible
for race conditions to occur.  By not using semaphores this increased my programs speed as all 4 children could 
be executing their calculations without having to wait for others.

I wrote out the matrix multiplication pseudocode before I wrote it in C to allow for me to figure out the most
efficient way to multiply matrices M and P together.

I decided to create a shared memory structure that had 3 double int arrays of size 4 to store the matrix values and a 
int array of size 4 to store the largest value in the row, saving me from having to do it in another for loop as the parent
 
I decided to calculated the largest value in each row, at the same time that the value was being calculated so 
that way there was no need for a redundent for loop, which would increase calculation time.

I tried to have minimal code in the children to allow for a faster recorded execution time.

************
Testing
************
I tested this program against 5 different matrix pairs.
I then found a matrix multiplier calculator on the internet (https://matrixcalc.org/en/) and compared the result of my 
program with the result of the calculated matrix on the internet.

3 example results are shown below:
-----
  1
-----
Matrix M:
    20     20     50     40
    10      6     70      8
    40      3      2     10
     8      7      6      5

Matrix N:
    10     30     50     70
     1      3      6      8
     9      5      5      7
     8      6      4      2

Matrix Q:								
   990   1150   1530   1990
   800    716    918   1254
   501   1279   2068   2858
   181    321    492    668

Largest Value = 2858 (CORRECT)

Internet Result:
   990   1150   1530   1990
   800    716    918   1254
   501   1279   2068   2858
   181    321    492    668

-----
  2
-----
Matrix M:
     5      2     12     40
    70     64      4     18
     8     33      9     12
     2      9     31      7

Matrix N:
     6     28      5     70
    13     38     33     43
     7     25      2     23
    13     53      1      2

Matrix Q:
   660   2636    155    792
  1514   5446   2488   7780
   696   2339   1159   2210
   437   1544    376   1254

Largest Value = 7780 (CORRECT)

Internet Result:
   660   2636    155    792
  1514   5446   2488   7780
   696   2339   1159   2210
   437   1544    376   1254

-----
  3
-----
Matrix M:
     1      1      1      1
     2      2      2      2
     3      3      3      3
     4      4      4      4

Matrix N:
     1      1      1      1
     2      2      2      2
     3      3      3      3
     4      4      4      4

Matrix Q:
    10     10     10     10
    20     20     20     20
    30     30     30     30
    40     40     40     40

Largest Value = 40 (CORRECT)

Internet Result:
    10     10     10     10
    20     20     20     20
    30     30     30     30
    40     40     40     40