#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>
#include <map>
#include <list>
#include <chrono>
#include <iomanip>

#include "order.cpp"
#include "orderBook.cpp"

using namespace std;
using namespace chrono;

//initiate order books for each instrument
OrderBook roseOrders;
OrderBook lavenderOrders;
OrderBook lotusOrders;
OrderBook tulipOrders;
OrderBook orchidOrders;

//map to store order books for each instrument
map<string, OrderBook> orderBooks = {
    {"Rose", roseOrders},
    {"Lavender", lavenderOrders},
    {"Lotus", lotusOrders},
    {"Tulip", tulipOrders},
    {"Orchid", orchidOrders},
};

// read csv file 
void readCSVFile(string, string);
// write data to csv file
void writeToCSVFile(ofstream &, Order);
// process an order 
list<Order> processOrder(Order);

int main()
{
    string finPath, foutPath = "OutputFiles/execution_rep.csv";

    cout << "Enter input file path: ";
    cin >> finPath;

    high_resolution_clock::time_point beginTime = high_resolution_clock::now();
    readCSVFile(finPath, foutPath);
    high_resolution_clock::time_point endTime = high_resolution_clock::now();

    duration<double> timeElapsed = duration_cast<duration<double>>(endTime - beginTime);
    cout << endl;
    cout << "Finished in: " << timeElapsed.count() << "s" << endl;
    cout << "Output file: " << foutPath;

    return 0;
}

// read csv file and write trades to output csv file
void readCSVFile(string finPath, string foutPath)
{
    fstream fin;
    ofstream fout;
    static string line; //static to save its value between function calls
    unsigned int i = 1;

    // open input csv file
    fin.open(finPath);

    // open output csv file 
    fout.open(foutPath);
    // set header for output file 
    fout << "Client Order ID,Order ID,Instrument,Side,Price,Quantity,Exec Status,Reason,Transaction Time\n";

    if (!fin.is_open())
    {
        cout << "Input data file not found";
        exit(0);
    }

    cout << "Processing..." << endl;

    getline(fin, line); // skip the first line (header)

    while (getline(fin, line))
    {
        Order order;
        stringstream lineStream(line);
        static string temp;

        // Generating a unique identifier for the order ID
        order.order_id = "ord" + to_string(i++);

        // Extract data from the CSV line
        getline(lineStream, order.client_id, ',');
        getline(lineStream, order.instrument, ',');

        temp = ""; //to reset temp
        getline(lineStream, temp, ',');
        order.side = atoi(temp.c_str());

        temp = "";
        getline(lineStream, temp, ',');
        order.quantity = atoi(temp.c_str());

        temp = "";
        getline(lineStream, temp, ',');
        order.price = atoi(temp.c_str());

        //validate the order
        order.validate();

        // if the order is rejected continue to the next iteration        if (order.status == 1)
        {
            writeToCSVFile(fout, order);
            continue;
        }

        // write the trades to output file 
        list<Order> trades = processOrder(order);
        for (Order &trade : trades)
        {
            writeToCSVFile(fout, trade);
        }
    }

    fin.close(); // close input file
    fout.close(); // close output file
}

list<Order> processOrder(Order order)
{
    OrderBook &relevantOrderBook = orderBooks[order.instrument]; //get the relevant order book
    return relevantOrderBook.insert(order);
}

void writeToCSVFile(ofstream &outputFile, Order order)
{   // Write data to the output file stream
    outputFile << order.client_id << ","
               << order.order_id << ","
               << order.instrument << ","
               << order.side << ","
               << order.price << ","
               << order.quantity << ","
               << Status[order.status] << ","
               << order.reason << ","
               << order.transaction_time
               << "\n";
}
