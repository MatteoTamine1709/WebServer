#ifndef HTTP_MIDDLEWARE_H
#define HTTP_MIDDLEWARE_H

#include <functional>

#include "Request.h"
#include "Response.h"

typedef std::function<void()> Next_t;
typedef void (*Middleware_t)(Request &, Response &, Next_t);
typedef std::pair<std::vector<std::string>, std::vector<std::string>> (
    *MiddlewareUseFunc_t)();
typedef std::string (*MiddlewareNameFunc_t)();

#endif