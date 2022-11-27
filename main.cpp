#include <iostream>
#include <istream>
#include <vector>
#include <bits/stdc++.h>
#include <queue>
#include <fstream>
using namespace std;


vector<vector<string>> getFlightPath(string startAirport, vector<string> stopAirports);
vector<string> getAirports(string city, string country);
vector<vector<string>> getDepartures(string airportCode);
vector<string> splitStrings(string str, string delim);
void mapJourney(vector<string> departureAirports, vector<string> arrivalAirports, string orderFileName);
void printTicket(vector<vector<string>> flightPath, string orderFileName);
void getOrder(string filename);



int main() {
    std::cout << "Hello, World!" << std::endl;
    //getOrder();
    getOrder("accra-lagos");
    return 0;
}


/**A method to get an order from a text file*/
void getOrder(string filename){
    string filepath = filename + ".txt";
    string myText;
    ifstream MyFile(filepath);

    vector<string> departure;
    vector<string> arrival;

    getline (MyFile, myText);
    departure.push_back(myText);
    getline (MyFile, myText);
    arrival.push_back(myText);


    MyFile.close();

    string startCity = splitStrings(departure[0], ",")[0];
    string startCountry = splitStrings(departure[0], ",")[1];

    string stopCity = splitStrings(arrival[0], ",")[0];
    string stopCountry = splitStrings(arrival[0], ",")[1];

    vector<string> departureAirports = getAirports(startCity, startCountry);
    vector<string> arrivalAirports = getAirports(stopCity, stopCountry);
    mapJourney(departureAirports, arrivalAirports, filename);
}


/**A method to split strings*/
vector<string> splitStrings(string str, string delim){
    string stringA;
    string stringB;
    int stringLength = str.size();

    vector<string> split;
    int start = 0;
    int end = str.find(delim);
    stringA = str.substr(start, end);
    stringB = str.substr(end+2, stringLength-(end+2));

    split.push_back(stringA);
    split.push_back(stringB);
    return split;
}


/**A class for flight Node*/
class FlightNode {

public:
    FlightNode *Parent;
    vector<string> State;
    int PathCost;

    FlightNode(vector<string> state, FlightNode *parent, int pathCost) {
        Parent = parent;
        State = state;
        PathCost = pathCost;
    }

    FlightNode(vector<string> state, int pathCost) {
        Parent = nullptr;
        State = state;
        PathCost = pathCost;
    }


    vector<vector<string>> solutionPath() {
        vector<vector<string>> goal_to_start;
        FlightNode current = *this;
        goal_to_start.push_back(current.State);

        while (current.Parent != NULL) {
            FlightNode obj = *current.Parent;
            goal_to_start.push_back(obj.State);
            current = *current.Parent;
        }
        reverse(goal_to_start.begin(), goal_to_start.end());
        return goal_to_start;
    }

    string toString() {
        cout << "Flight with state " << State[1] << " " <<State[3] << "  " <<State[5] << " and parent " << Parent << "with number of stops"
             << PathCost;
    }
};



/** A structure to compare two nodes */
struct comparator{
    bool operator()(FlightNode n1, FlightNode n2){
        return n1.PathCost > n2.PathCost;
    }
};


//Mapping Journeys
void mapJourney(vector<string> departureAirports, vector<string> arrivalAirports, string orderFileName){
    for (string airport : departureAirports) {
        cout << "These are your departure airports" << airport;
        vector<vector<string>> flightPath = getFlightPath(airport, arrivalAirports);
        printTicket(flightPath, orderFileName);
    }
}


//Getting  a flight
vector<vector<string>> getFlightPath(string startAirport, vector<string> stopAirports){

    //A vector containing arrival airports

    vector<string> arrivalAirports = stopAirports;

    //A vector containing routes of departing flights

    vector<vector<string>> departingFlights = getDepartures(startAirport);

    // Priority queue to store valid flight nodes
    priority_queue<FlightNode, vector<FlightNode>, comparator> FlightMatches;

    // For each flight perform a depth-first search, if a solution is found
    // add the node to the priorityQueue
    while (FlightMatches.size() < 5) {
        for (vector<string> flight: departingFlights) {

            //Creating a starting node with no parent, zero cost
            FlightNode node(flight, 0);

            // Check if the current flight will arrive in a destination airport
            if (count(arrivalAirports.begin(), arrivalAirports.end(), node.State[4]) !=0){
                cout << "FOUND A FLIGHT, ADDING TO PRIORITY QUEUE";
                FlightMatches.push(node);
            }

            //A set which hold routes that have been visited
            set<vector<string>> visited;

            //A vector representing an unexplored frontier
            vector<FlightNode*> neighbourhood;

            //Push the memory address of the node into the frontier for future exploration
            neighbourhood.push_back(&node);

            //Exploring the frontier
            while (!neighbourhood.empty() && FlightMatches.size() < 5) {

                //De-referencing the pointers being stored in neighbourhood to get the actual node object
                node = *neighbourhood.front();
                neighbourhood.erase(neighbourhood.begin());

                //Insert node into visited set
                visited.insert(node.State);

                //Create a vector containing reachable locations from given flight node
                vector<vector<string>> neighbours = getDepartures(node.State[4]);

                if (neighbours.size() != 0) {
                    for (vector<string> neighbour: neighbours) {
                        vector<vector<string>> childPath = node.solutionPath();
                        childPath.push_back(neighbour);

                        // Create a new child node
                        FlightNode child(neighbour, &node, node.PathCost + 1);

                        if (visited.count(child.State) == 0) {
                            if (count(neighbourhood.begin(), neighbourhood.end(), &child) == 0) {
                                if (!count(arrivalAirports.begin(), arrivalAirports.end(), child.State[4])) {
                                    FlightMatches.push(child);
                                    cout << "FOUND A FLIGHT, ADDING TO PRIORITY QUEUE";
                                }
                                //neighbourhood.push_back(&child);
                            }
                        }
                    }
                }
            }
        }
    }
    cout << "DONE! PLEASE CHECK FOR YOUR TICKET PRINTOUT!";
    FlightNode flight = FlightMatches.top();
    return flight.solutionPath();
}



/** Print ticket method*/
void printTicket(vector<vector<string>> flightPath, string orderFileName){
    string outputFile = orderFileName + "_output.txt";
    ofstream ticket(outputFile);
    if(ticket.is_open()){
        ticket << "TICKET PRINT-OUT\n";
    }
    if(!ticket.fail()){
        cout << "Ticket Created successfully\n";
    }
    for (int i = 0; i < flightPath.size(); i++) {
        string airline = flightPath[i][1];
        string departure = flightPath[i][2];
        string arrival = flightPath[i][4];
        ticket << airline << " FROM " << departure << " TO " << arrival << endl;
    }
    ticket << "TOTAL FLIGHTS" << " " <<  flightPath.size() << endl << "TOTAL ADDITIONAL STOPS: 0";
    ticket << "------------------------------" << endl << endl;
    ticket.close();
}



/** Get Departures method*/
vector<vector<string>> getDepartures(string airportCode){
    vector<vector<string>> departures;

    vector<string> cells;
    vector<vector<string>> records;

    ifstream route;
    route.open("routes.csv");

    if(route.is_open()){
        cout << "File open\n";
    }
    if(route.fail()){
        cerr << "Unable to open file" << endl;
    }

    while (route.peek() !=EOF){
        string cell;
        getline(route, cell, ',');
        cells.push_back(cell);
    }
    route.close();

    //A way to get a collection of the records
    int j = 0;
    int k = 0;
    while(k < cells.size()/8){
        vector<string> record;
        for(int i=j; i<=j+8; i++){
            record.push_back(cells[i]);
        }
        j+=8;
        k++;
        records.push_back(record);
    }

    //A way of collecting airport codes

    for(vector<string> v : records){
        if(airportCode == v[2]){
            departures.push_back(v);
        }
    }
    return departures;
}

/** Get airport codes*/
vector<string> getAirports(string city, string country) {
    cout << "Getting airport codes";
    vector<string> airportCodes;


    vector<string> cells;
    vector<vector<string>> records;

    ifstream airports;
    airports.open("airports.csv");

    if(airports.is_open()){
        cout << "File open\n";
    }
    if(airports.fail()){
        cerr << "Unable to open file" << endl;
    }

    while (airports.peek() !=EOF){
        string cell;
        getline(airports, cell, ',');
        cells.push_back(cell);
    }
    airports.close();

    //A way to get a collection of the records
    int j = 0;
    int k = 0;
    while(k < cells.size()/13){
        vector<string> record;
        for(int i=j; i<=j+13; i++){
            record.push_back(cells[i]);
        }
        j+=13;
        k++;
        records.push_back(record);
    }

    //A way of collecting airport codes

    for(vector<string> v : records){
        if(city == v[2] && country == v[3]){
            airportCodes.push_back(v[4]);
        }
    }
    return airportCodes;
}





