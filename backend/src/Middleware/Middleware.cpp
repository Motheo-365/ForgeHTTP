#include "Middleware/Middleware.h"

Middleware* Middleware::setNext(Middleware* m) {
    return this->next = m;
}