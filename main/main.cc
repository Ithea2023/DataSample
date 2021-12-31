#include <iostream>
#include "unit_base.h"
#include "main.h"
int main()
{

    LOG_MING << "hello, max(" << 12 << ", " << -5 << ")=" << ::max(12, -5);
    return 0;
}
