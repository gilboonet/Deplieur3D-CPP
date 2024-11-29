#include "structures.h"

Nums::Nums() {}
Nums::Nums(int n1, int n2) : id1(n1), id2(n2) {}
Nums::Nums(int n1, int n2, int num) : id1(n1), id2(n2), num(num) {}
bool Nums::operator==(const Nums &n) const
{
    return
        (this->id1 == n.id1 && this->id2 == n.id2)
     || (this->id1 == n.id2 && this->id2 == n.id1);
}
