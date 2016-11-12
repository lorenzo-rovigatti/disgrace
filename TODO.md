* ~~All the methods that build/add sets should take as arguments (or return) a list of sets, since we want to support the convention that a file can contain multiple data sets separated by one or more empty lines~~
* Implement exception handling and remove exit(...) statements
* Remove a curve from the legend if it has an empty legend title
* Fix memory leaks (according to valgrind, heap usage at exit is very high, is this normal for qt applications?)
* Add acknowledgements to Google (for material icons) and Michael Goerz (for the xmgrace_parser.py) in the "about" dialog
* ~~Check the name of the file that is created when disgrace crashes~~