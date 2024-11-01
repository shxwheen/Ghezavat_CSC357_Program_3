# shows dir tree

I used `fs` directory for testing

`printDir` 
1. This function opens the directory specifed by the user
2. Then it reads the entries within the directory; it skips hidden filese unless the `-a`flag is high; creates `dirElement` struct to represent an element of the user-specified directory 
3. `dirElement` gets populated with appropiate information (checks attributes with `stat` function and get fileInfo)
4. Allocate and construct the path for `dirElement` using `sprintf` to store 
5. Resizing array and reallocing for next elements 
6. Once array populated, using quicksort to sort the dirElements
7. Iterate over the sorted entries and print tree with appropiate indentation, files, size and hidden name (on request)
8. For the dirElements in the array that are (sub)directories (not child or parent `.` and `..`), updated path is created and recursive call made to `printDir` and increase indentation bc it's a subdirectory


//note to self
recursive part:

`printDir` called for subdirectory and starts listing contents of it; if another sub directory is encountered, it calls itself for the new subdirectories as well, each time sub directory encountered indent increases

base case : when no further subdirectories are left

after reaching a base case, the function completes for that specific sub directory, returns to the previous level of recursion and continues with the next dirElements on that level of recursion until it's gone through every dirElement
