#include "Controllers/CalculatorController.h"

#include <cmath>
#include <nlohmann/json.hpp>

namespace {
using json = nlohmann::json;

HttpResponse badRequest(const std::string& message) {
    HttpResponse response = HttpResponse::json(json{{"error", message}}.dump());
    response.setStatusCode(400);
    return response;
}
}

HttpResponse CalculatorController::calculate(const HttpRequest& request) {
    json body;
    try {
        body = json::parse(request.getBody());
    } catch (const json::exception&) {
        return badRequest("Request body must be valid JSON");
    }

    if (!body.is_object() ||
        !body.contains("left") || !body.at("left").is_number() ||
        !body.contains("right") || !body.at("right").is_number() ||
        !body.contains("operator") || !body.at("operator").is_string()) {
        return badRequest(
            "Expected numeric left and right values and a string operator");
    }

    const double left = body.at("left").get<double>();
    const double right = body.at("right").get<double>();
    const std::string operation = body.at("operator").get<std::string>();
    if (!std::isfinite(left) || !std::isfinite(right)) {
        return badRequest("Operands must be finite numbers");
    }

    double result = 0.0;
    if (operation == "+") {
        result = left + right;
    } else if (operation == "-") {
        result = left - right;
    } else if (operation == "*") {
        result = left * right;
    } else if (operation == "/") {
        if (right == 0.0) {
            return badRequest("Division by zero is not allowed");
        }
        result = left / right;
    } else {
        return badRequest("Operator must be one of +, -, *, or /");
    }

    if (!std::isfinite(result)) {
        return badRequest("Calculation result is outside the supported range");
    }

    return HttpResponse::json(json{
        {"left", left},
        {"operator", operation},
        {"right", right},
        {"result", result}
    }.dump());
}
