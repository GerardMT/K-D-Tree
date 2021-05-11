# K-D Tree
A simple generic implementation of a k-d tree.

A more complete documentation can be found [here](docs/report/report.pdf).

## Implemented operations
- Test whether the collection is empty. *O(1)*.
- Size of the collection. *O(1)*.
- Insert an element. *O(log n)*. 
- Erase an element. *O(log n)*.
- Erase all elements. *O(1)*.
- Test whether the collection contains an element. *O(log n)*.
- Get the element with the minimum dimension. *O(n)*.
- Get the elements that are inside a given range. *O(n + m)*.
- Get the nearest neighbor element of a given element. *O(log n)*.

## Provided example
### Build and run
Clone the repository:

	git clone https://github.com/GerardMT/K-D-Tree
	cd K-D-Tree

Compile the code:

	mkdir build
	cd build
	cmake -DCMAKE_BUILD_TYPE=Release ../
	make

Simply run:

	./k-d-tree

### Execution output
```
insert (7, 2): 1
0:    (7, 2)0    

insert (5, 4): 1
0:            (7, 2)0            
1:    (5, 4)1    

insert (9, 6): 1
0:            (7, 2)0            
1:    (5, 4)1       (9, 6)1    

insert (2, 3): 1
0:                           (7, 2)0                           
1:            (5, 4)1                       (9, 6)1            
2:    (2, 3)0    

insert (4, 7): 1
0:                           (7, 2)0                           
1:            (5, 4)1                       (9, 6)1            
2:    (2, 3)0       (4, 7)0    

insert (8, 1): 1
0:                           (7, 2)0                           
1:            (5, 4)1                       (9, 6)1            
2:    (2, 3)0       (4, 7)0       (8, 1)0    

insert (8, 1): 0
0:                           (7, 2)0                           
1:            (5, 4)1                       (9, 6)1            
2:    (2, 3)0       (4, 7)0       (8, 1)0    

contains (8, 1): 1

erase (8, 1): 1
0:                           (7, 2)0                           
1:            (5, 4)1                       (9, 6)1            
2:    (2, 3)0       (4, 7)0    

contains (8, 1): 0

min 0: (2, 3)
min 1: (7, 2)

range (2, 3), (9, 6): (2, 3), (5, 4)

nerares neighbor (1, 1): (2, 3)

clear
<empty tree>

empty: 1

Inserting 1000 random points. Saving results on insert.dat

Searching (contains) 1000 random points. Saving results on contains.dat
```