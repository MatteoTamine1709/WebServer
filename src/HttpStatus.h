#ifndef HTTP_STATUS_H
#define HTTP_STATUS_H

#include <string>
#include <unordered_map>

#define CONTINUE 100
#define SWITCHING_PROTOCOLS 101
#define PROCESSING 102
#define EARLY_HINTS 103
#define OK 200
#define CREATED 201
#define ACCEPTED 202
#define NON_AUTHORITATIVE_INFORMATION 203
#define NO_CONTENT 204
#define RESET_CONTENT 205
#define PARTIAL_CONTENT 206
#define MULTI_STATUS 207
#define ALREADY_REPORTED 208
#define IM_USED 226
#define MULTIPLE_CHOICES 300
#define MOVED_PERMANENTLY 301
#define FOUND 302
#define SEE_OTHER 303
#define NOT_MODIFIED 304
#define USE_PROXY 305
#define SWITCH_PROXY 306
#define TEMPORARY_REDIRECT 307
#define PERMANENT_REDIRECT 308
#define BAD_REQUEST 400
#define UNAUTHORIZED 401
#define PAYMENT_REQUIRED 402
#define FORBIDDEN 403
#define NOT_FOUND 404
#define METHOD_NOT_ALLOWED 405
#define NOT_ACCEPTABLE 406
#define PROXY_AUTHENTICATION_REQUIRED 407
#define REQUEST_TIMEOUT 408
#define CONFLICT 409
#define GONE 410
#define LENGTH_REQUIRED 411
#define PRECONDITION_FAILED 412
#define PAYLOAD_TOO_LARGE 413
#define URI_TOO_LONG 414
#define UNSUPPORTED_MEDIA_TYPE 415
#define RANGE_NOT_SATISFIABLE 416
#define EXPECTATION_FAILED 417
#define IM_A_TEAPOT 418
#define MISDIRECTED_REQUEST 421
#define UNPROCESSABLE_ENTITY 422
#define LOCKED 423
#define FAILED_DEPENDENCY 424
#define TOO_EARLY 425
#define UPGRADE_REQUIRED 426
#define PRECONDITION_REQUIRED 428
#define TOO_MANY_REQUESTS 429
#define REQUEST_HEADER_FIELDS_TOO_LARGE 431
#define UNAVAILABLE_FOR_LEGAL_REASONS 451
#define INTERNAL_SERVER_ERROR 500
#define NOT_IMPLEMENTED 501
#define BAD_GATEWAY 502
#define SERVICE_UNAVAILABLE 503
#define GATEWAY_TIMEOUT 504
#define HTTP_VERSION_NOT_SUPPORTED 505
#define VARIANT_ALSO_NEGOTIATES 506
#define INSUFFICIENT_STORAGE 507
#define LOOP_DETECTED 508
#define NOT_EXTENDED 510
#define NETWORK_AUTHENTICATION_REQUIRED 511

static std::unordered_map<int, std::string> HTTP_MESSAGES = {
    {100, "Continue"},
    {101, "Switching Protocols"},
    {102, "Processing"},
    {103, "Early Hints"},
    {200, "OK"},
    {201, "Created"},
    {202, "Accepted"},
    {203, "Non-Authoritative Information"},
    {204, "No Content"},
    {205, "Reset Content"},
    {206, "Partial Content"},
    {207, "Multi-Status"},
    {208, "Already Reported"},
    {226, "IM Used"},
    {300, "Multiple Choices"},
    {301, "Moved Permanently"},
    {302, "Found"},
    {303, "See Other"},
    {304, "Not Modified"},
    {305, "Use Proxy"},
    {306, "Switch Proxy"},
    {307, "Temporary Redirect"},
    {308, "Permanent Redirect"},
    {400, "Bad Request"},
    {401, "Unauthorized"},
    {402, "Payment Required"},
    {403, "Forbidden"},
    {404, "Not Found"},
    {405, "Method Not Allowed"},
    {406, "Not Acceptable"},
    {407, "Proxy Authentication Required"},
    {408, "Request Timeout"},
    {409, "Conflict"},
    {410, "Gone"},
    {411, "Length Required"},
    {412, "Precondition Failed"},
    {413, "Payload Too Large"},
    {414, "URI Too Long"},
    {415, "Unsupported Media Type"},
    {416, "Range Not Satisfiable"},
    {417, "Expectation Failed"},
    {418, "I'm a teapot"},
    {421, "Misdirected Request"},
    {422, "Unprocessable Entity"},
    {423, "Locked"},
    {424, "Failed Dependency"},
    {425, "Too Early"},
    {426, "Upgrade Required"},
    {428, "Precondition Required"},
    {429, "Too Many Requests"},
    {431, "Request Header Fields Too Large"},
    {451, "Unavailable For Legal Reasons"},
    {500, "Internal Server Error"},
    {501, "Not Implemented"},
    {502, "Bad Gateway"},
    {503, "Service Unavailable"},
    {504, "Gateway Timeout"},
    {505, "HTTP Version Not Supported"},
    {506, "Variant Also Negotiates"},
    {507, "Insufficient Storage"},
    {508, "Loop Detected"},
    {510, "Not Extended"},
    {511, "Network Authentication Required"}};

struct HttpStatus {
    int code;
    std::string message;

    HttpStatus(int code) : code(code), message(HTTP_MESSAGES[code]) {}

    HttpStatus(int code, std::string message) : code(code), message(message) {}

    bool operator==(const HttpStatus &other) const {
        return code == other.code;
    }

    bool operator!=(const HttpStatus &other) const {
        return code != other.code;
    }
};

#endif  // HTTP_STATUS_H