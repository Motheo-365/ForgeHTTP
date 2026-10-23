#ifndef CALCULATORCONTROLLER_H
#define CALCULATORCONTROLLER_H

#include "HTTP/HttpRequest.h"
#include "HTTP/HttpResponse.h"

class CalculatorController {
public:
    static HttpResponse calculate(const HttpRequest& request);
};

#endif
