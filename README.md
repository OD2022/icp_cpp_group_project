This project is intended to help a user who has a start city and stop city, to find an optimal flight from a database of airports, routes and airline CSV files. It uses an iterative depth-first search approach to find an optimal flight based on the number of flight stops.

This project and all it's source code is contained in the main.cpp file. 
To place a flight request or order, simply write in a text file your departure and arrival, 
for example:

Accra, Ghana
Lagos, Nigeria

Pass the name of your text file without the extension into the getOrder method. For example, if your text file is named "accra-lagos.txt", do this:
getOrder("accra-lagos")

Your ticket should be printed out in a text file called "accra-lagos_output.txt", in the same directory you are working in.
