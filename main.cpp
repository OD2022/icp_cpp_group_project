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
    getOrder("accra-lagos");
    return 0;
}
/**A class for flight Node*/
class FlightNode {

public:
    FlightNode* Parent;
    vector<string> State;
    int PathCost;

    //Constructor
    FlightNode(vector<string> state, FlightNode* parent, int pathCost) {
        Parent = parent;
        State = state;
        PathCost = pathCost;
    }

    /**A method to get the solution path of a node*/
    vector<vector<string>> solutionPath() {
        vector<vector<string>> goal_to_start;
        FlightNode current = *this;
        goal_to_start.push_back(current.State);

        while(current.Parent != NULL) {
            FlightNode* parentPointer = current.Parent;
            FlightNode parentObject = *parentPointer;
            goal_to_start.push_back(parentObject.State);
            current = parentObject;
        }
        reverse(goal_to_start.begin(), goal_to_start.end());
        return goal_to_start;
    }

    /**A method to print a string*/
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


/** Print ticket method*/
void printTicket(vector<vector<string>> flightPath, string orderFileName){
    try{
        string outputFile = orderFileName + "_output.txt";
        ofstream ticket(outputFile);
        if(ticket.is_open()){
            ticket << "TICKET PRINT-OUT\n";
        }
        if(!ticket.fail()){
            cout << "Ticket Created successfully\n";
        }
        for (int i = 0; i < flightPath.size(); i++) {
            string airline = flightPath[i][0];
            string departure = flightPath[i][2];
            string arrival = flightPath[i][4];
            ticket << airline << " FROM " << departure << " TO " << arrival << endl;
        }
        ticket << "TOTAL FLIGHTS" << " " <<  flightPath.size() << endl << "TOTAL ADDITIONAL STOPS: 0";
        ticket << "------------------------------" << endl << endl;
        ticket.close();
        } catch(exception const& e){
            cout << "There was an error: " << e.what() << endl;
        }
    }


/** Get Departures method*/
vector<vector<string>> getDepartures(string airportCode){
    string record;
    vector<vector<string>> departures;
    vector<string> cleanedRecord;
    vector<vector<string>> cleanedRecords;

    try{
        ifstream route;
        route.open("routes.csv");
        while (route.peek() != EOF) {
            getline(route, record);
            const char separator = ',';
            stringstream streamData(record);
            string cell;
            while(getline(streamData, cell, separator)){
                cleanedRecord.push_back(cell);
            }
            cleanedRecords.push_back(cleanedRecord);
            cleanedRecord.clear();
        }
        for(vector<string> flight : cleanedRecords){
            if(airportCode == flight[2]){
                departures.push_back(flight);
            }
        }
        route.close();
    } catch(exception const& e) {
        cout << "There was an error: " << e.what() << endl;
        }
    return departures;
}


/** Get airport codes*/
vector<string> getAirports(string city, string country){
    cout << "Getting airport codes" << endl;

    vector<string> airportCodes;
    string record;
    vector<string> cleanedRecord;
    vector<vector<string>> airportDetails;

    try{
        ifstream airports;
        airports.open("airports.csv");
        while (airports.peek() != EOF) {
            getline(airports, record);
            const char separator = ',';
            stringstream streamData(record);
            string cell;
            while(getline(streamData, cell, separator)){
                cleanedRecord.push_back(cell);
            }
            airportDetails.push_back(cleanedRecord);
            cleanedRecord.clear();
        }
        for(vector<string> airportDetail : airportDetails){
            if(city == airportDetail[2] && country == airportDetail[3]){
                airportCodes.push_back(airportDetail[4]);
            }
        }
        airports.close();
        }catch (exception const& e){
            cout << "There was an error: " << e.what() << endl;
        }
        return airportCodes;
    }


/**Function to get a flight*/
vector<vector<string>> getFlightPath(string startAirport, vector<string> stopAirports){

    //A vector containing arrival airports
    vector<string> arrivalAirports = stopAirports;

    //A vector containing departing flights from start airport
    vector<vector<string>> departingFlights = getDepartures(startAirport);

    // Priority queue to store flight matches
    priority_queue<FlightNode, vector<FlightNode>, comparator> FlightMatches;

    // For each flight perform a depth-first search, if a solution is found
    // add the node to the priorityQueue
    while (FlightMatches.empty()){
        for (vector<string> flight: departingFlights){

            //Creating a starting node with no parent, zero cost
            FlightNode node(flight, NULL, 0);

            // Check if the current flight will arrive in a destination airport
            if (count(arrivalAirports.begin(), arrivalAirports.end(), node.State[4]) > 0){
                cout << "FOUND A STRAIGHT FLIGHT, ADDING TO PRIORITY QUEUE" << endl;
                FlightMatches.push(node);
                break;
            } else{
                //A set which hold routes that have been visited
                set<vector<string>> visited;
                //A vector representing an unexplored frontier
                vector<FlightNode> neighbourhood;
                //Push the node into the frontier for future exploration
                neighbourhood.push_back(node);

                //Exploring the frontier
                while (!neighbourhood.empty() && FlightMatches.empty()){
                    //Pop the first node in the frontier
                    node = neighbourhood.front();
                    neighbourhood.erase(neighbourhood.begin());
                    //Insert node into visited set
                    visited.insert(node.State);

                    //Generate a list of places reachable from current node
                    vector<vector<string>> neighbours = getDepartures(node.State[4]);

                    if (!neighbours.empty()){
                        for (vector<string> neighbour: neighbours){
                            // Create a new child node
                            FlightNode child(neighbour, &node, node.PathCost + 1);
                            if(visited.count(child.State) == 0){
                                if (count(arrivalAirports.begin(), arrivalAirports.end(), child.State[4]) > 0){
                                        FlightMatches.push(child);
                                        cout << "FOUND AN INNER FLIGHT, ADDING TO PRIORITY QUEUE" << endl;
                                        break;
                                    }neighbourhood.push_back(child);
                            }
                        }
                    }
                }
            }
        }
    }
    cout << "DONE! PLEASE CHECK FOR YOUR TICKET PRINTOUT!" << endl;
    FlightNode flight = FlightMatches.top();
    return flight.solutionPath();
}

/**A function to map a journey and print a ticket for the journey*/
void mapJourney(vector<string> departureAirports, vector<string> arrivalAirports, string orderFileName){
    for (string airport : departureAirports){
        vector<vector<string>> flightPath = getFlightPath(airport, arrivalAirports);
        printTicket(flightPath, orderFileName);
    }
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







