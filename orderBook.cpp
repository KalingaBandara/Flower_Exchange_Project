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

void add_buy(list<Order> &, Order);
void add_sell(list<Order> &, Order);

class OrderBook
{
public:
    // Public member variables
    list<Order> buy_side;
    list<Order> sell_side;

    // insert a new order to the book 
    list<Order> insert(Order order);

    // display the current orders
    void display();
};

list<Order> OrderBook::insert(Order order)
{
    list<Order> trades;

    // if order is buy 
    if (order.side == 1)
    {
        list<Order>::iterator itr;
        for (itr = sell_side.begin(); itr != sell_side.end(); ++itr)
        {
            Order sell_order = *itr; // copy the sell_side order
            Order buy_order = order; // copy the incoming buy_side order
            int quantity;

            //  if the price of the current sell-side order is greater than the incoming buy-side order; stop loop 
            if (itr->price > order.price)
                break;

            // if sell order has some stock 
            if (order.quantity >= itr->quantity)
            {
                order.quantity -= itr->quantity;
                quantity = itr->quantity;
                itr->quantity = 0; // Quantity of sell_side order is zero 
            }
            // if sell order has more stock 
            else if (itr->quantity >= order.quantity)
            {
                itr->quantity = order.quantity;
                quantity = order.quantity;
                order.quantity = 0; // Quantity of incoming buy_side order is zero
            }
            else
            {
                break;
            }

            // update orders
            itr->update();
            order.update();

            // set the current buy trade info 
            buy_order.status = order.status;
            buy_order.price = itr->price;
            buy_order.quantity = quantity;
            buy_order.set_transaction_time();

            // set the current sell trade info 
            sell_order.status = itr->status;
            sell_order.price = itr->price;
            sell_order.quantity = quantity;
            sell_order.set_transaction_time();

            // add the current order to trades list 
            trades.push_back(buy_order);
            trades.push_back(sell_order);

            // if the seller order is complete remove from order book 
            if (itr->status == 2)
                itr = --sell_side.erase(itr);

            // stop processing if no stocks left to buy
            if (order.quantity == 0)
                break;
        }

        /* set order transaction time */
        order.set_transaction_time();

        /* if the order is not rejected and not filled add to the book */
        if (order.status != 1 && order.status != 2)
            add_buy(buy_side, order);

        /* if the order is new */
        if (order.status == 0)
            trades.push_back(order);
    }

    // if the order is sell 
    if (order.side == 2)
    {
        list<Order>::iterator itr;
        for (itr = buy_side.begin(); itr != buy_side.end(); ++itr)
        {
            Order buy_order = *itr;
            Order sell_order = order;
            int quantity;

            /* if the sell order price is greater; stop loop */
            if (order.price > itr->price)
                break;

            /* if sell order has more stock */
            if (order.quantity >= itr->quantity)
            {
                order.quantity -= itr->quantity;
                quantity = itr->quantity;
                itr->quantity = 0;
            }
            /* if sell order has some stock */
            else if (itr->quantity >= order.quantity)
            {
                itr->quantity -= order.quantity;
                quantity = order.quantity;
                order.quantity = 0;
            }
            else
            {
                break;
            }

            // update orders 
            itr->update();
            order.update();

            // set the current buy trade info 
            buy_order.status = itr->status;
            buy_order.price = itr->price;
            buy_order.quantity = quantity;
            buy_order.set_transaction_time();

            // set the current sell trade info 
            sell_order.status = order.status;
            sell_order.price = itr->price;
            sell_order.quantity = quantity;
            sell_order.set_transaction_time();

            // add the current order to trades list 
            trades.push_back(sell_order);
            trades.push_back(buy_order);

            // if the buyer order is complete remove from order book
            if (itr->status == 2)
                itr = --buy_side.erase(itr);

            // stop processing if no stocks 
            if (order.quantity == 0)
                break;
        }

        // set order transaction time 
        order.set_transaction_time();

        // if the order is not rejected and not filled add to the book 
        if (order.status != 1 && order.status != 2)
            add_sell(sell_side, order);

        // if the order is new 
        if (order.status == 0)
            trades.push_back(order);
    }

    return trades;
}
void OrderBook::display()
{
    cout << "=== Buy Side ===\n" << endl;
    list<Order>::iterator b_itr;
    for (b_itr = buy_side.begin(); b_itr != buy_side.end(); ++b_itr)
    {
        b_itr->display();
    }
    cout << endl;

    cout << "\n=== Sell Side ===\n" << endl;
    list<Order>::iterator s_itr;
    for (s_itr = sell_side.begin(); s_itr != sell_side.end(); ++s_itr)
    {
        s_itr->display();
    }
    cout << endl;
}


// to insert buy orders in ascending order of price
void add_buy(list<Order> &buyOrders, Order order) 
{
    list<Order>::iterator itr;
    for (itr = buyOrders.begin(); itr != buyOrders.end(); ++itr)
    {
        if (order.price >= itr->price)
        {
            buyOrders.insert(itr, order);
            return;
        }
    }
    buyOrders.push_back(order);
}

// to insert sell orders in descending order of price
void add_sell(list<Order> &sellOrders, Order order)
{
    list<Order>::iterator itr;
    for (itr = sellOrders.begin(); itr != sellOrders.end(); ++itr)
    {
        if (itr->price >= order.price)
        {
            sellOrders.insert(itr, order);
            return;
        }
    }
    sellOrders.push_back(order);
}