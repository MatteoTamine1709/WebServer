#ifndef HTTP_MIDDLEWARE_H
#define HTTP_MIDDLEWARE_H

#include "Request.h"
#include "Response.h"
#include <functional>

typedef std::function<void()> Next_t;
typedef void (*Middleware_t)(const Request &, Response &, Next_t);

#endif