#include <fstream>
#include <iostream>
#include <sstream>

#include "src/CssBuilder.h"
#include "src/HtmlBuilder.h"
#include "src/Request.h"
#include "src/Response.h"

extern "C" {
void get(Request &req, Response &res);
}

void get(Request &req, Response &res) {
    CssBuilder css{};
    css.createSelector("li").addProperty("background-color", "red");
    HtmlBuilder html{};
    html.style().text(css.toString()).style_();
    html.h3().text("Query").h3_();
    html.ul();
    for (const auto &[key, value] : req.query)
        html.li().text(key + ": " + value).li_();
    html.ul_();

    html.h3().text("Parameters").h3_();
    html.ul();
    for (const auto &[key, value] : req.params)
        html.li().text(key + ": " + value).li_();
    html.ul_();

    res.send(html.toString());
}