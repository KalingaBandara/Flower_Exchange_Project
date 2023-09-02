#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>
#include <map>
#include <list>
#include <chrono>
#include <iomanip>

using namespace std;
using namespace chrono;

// possible instruments
string Instruments[5] = {"Rose",
                         "Lavender",
                         "Lotus", 
                         "Tulip", 
                         "Orchid"};
// possible status
map<int, string> Status = {{0, "New"},
                           {1, "Rejected"},
                           {2, "Fill"},
                           {3, "Pfill"}};

//check if the given value is in array or not
template <class ContainerType, typename ElementType>  //to work with a wide range of container types and element types.
bool customContains(ContainerType &&container, ElementType elementToFind)
{
    return find(begin(container), end(container), elementToFind) != end(container);
};

/* get milliseconds till now since epoch time  
time_t get_time()
{
    return system_clock::to_time_t(system_clock::now());
}*/

class Order
{
public:
    // Public member variables
    string client_id;
    string order_id;
    string instrument;
    int side;
    double price;
    int quantity;
    int status;
    string reason;
    string transaction_time;

    // Constructor to initialize the order
    Order()
    {
        status = 0; // new
        set_transaction_time();
    };

    // validate the order
    void validate();
    // validate the order
    void update();
    // display the order
    void display();
    // set transaction time
    void set_transaction_time();
};

void Order::validate()
{
    if (client_id.empty())
    {
        reason = "Client ID not defined";
    }

    if (!customContains(Instruments, instrument))
    {
        reason = "Invalid instrument type!";
    }

    if (side != 1 && side != 2)
    {
        reason = "Invalid side value!";
    }

    if (price <= 0)
    {
        reason = "Invalid price value!";
    }

    if (quantity % 10 != 0 || quantity > 1000 || quantity <= 0)
    {
        reason = "Invalid quantity value!";
    }

    if (!reason.empty())
    {
        status = 1; // rejected
    }
}

void Order::update()
{
    if (quantity > 0)
    {
        status = 3; // partial fill
    }

    if (quantity == 0)
    {
        status = 2;
    };
}

void Order::set_transaction_time()
{
    ostringstream oss;
    auto now = system_clock::now();
    const auto as_time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

    oss << put_time(localtime(&as_time_t), "%Y%m%d-%H%M%S%T");
    oss << '.' << setfill('0') << setw(3) << ms.count();

    transaction_time = oss.str();
}

void Order::display()
{
    cout << "Client ID: " << client_id << endl;
    cout << "Order ID: " << order_id << endl;
    cout << "Instrument: " << instrument << endl;
    cout << "Side: " << side << endl;
    cout << "Price: " << price << endl;
    cout << "Quantity: " << quantity << endl;
    cout << "Status: " << Status[status] << endl;
    cout << "Reason: " << reason << endl;
    cout << "Transaction Time: " << transaction_time << endl;
    cout << endl;
}