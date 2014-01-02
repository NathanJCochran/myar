myar
----

Myar is a simple archive utility written in C.  It provides options for
appending, extracting, and deleting files from an archive, as well as for
listing the contents of an archive.

The archive files it produces are compatible with the standard `ar` tool.

#### Compilation:
`make myar`

#### Usage:

    myar {-q|-A|-x|-d|-t|-v} <archive file> [member files...]
    
    Options:  
    -q - Quickly append the specified member files to the archive.  
    -A - Append all files in the current directory to the archive  
    -x - Extract the specified member files from the archive. If no member files are specified, extract all.  
    -d - Delete the first instance of each of the specified member files from the archive  
    -t - Display a table of contents for the archive  
    -v - Display a verbose table of contents for the archive  
