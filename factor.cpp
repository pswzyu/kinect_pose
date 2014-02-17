#include "factor.h"
#include <iostream>

using namespace std;

Factor::Factor()
{
}

void Factor::print()
{
    cout << "var:" << endl;
    var.print();
    cout << endl << "card:" << endl;
    card.print();
    cout << endl << "val:" << endl;
    val.print();
}
