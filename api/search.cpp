#include <fstream>
#include <iostream>
#include <sstream>

#include "src/CssBuilder.h"
#include "src/HtmlBuilder.h"
#include "src/Request.h"
#include "src/Response.h"

extern "C" {
void get(const Request &req, Response &res);
}

void get(const Request &req, Response &res) {
    CssBuilder css{};
    css.createSelector("#searchResult").addProperty("background-color", "red");
    HtmlBuilder html{};
    html.style().text(css.toString()).style_();
    html.ul();
    for (int i = 0; i < 10; ++i) {
        html.li().text("Item " + req.params.at("query")).li_();
    }
    html.ul_();
    res.send(html.toString());
}