#include <fstream>
#include <iostream>
#include <sstream>

#include "src/CssBuilder.h"
#include "src/HtmlBuilder.h"
#include "src/Request.h"
#include "src/Response.h"

extern "C" {
void get(const Request &req, Response &res);
void post(const Request &req, Response &res);
}

void get(const Request &req, Response &res) {
    CssBuilder css{};
    css.createSelector("body")
        .addProperty("background-color", "#f1f1f1")
        .addProperty("font-family", "Verdana, sans-serif")
        .addProperty("font-size", "20px")
        .addProperty("line-height", "1.5")
        .addProperty("margin", "0")
        .addProperty("padding", "0")
        .addProperty("box-sizing", "border-box")
        .addProperty("display", "flex")
        .addProperty("justify-content", "center")
        .addProperty("align-items", "center")
        .addProperty("min-height", "100vh")
        .addProperty("color", "#666");
    css.createSelector("div.container")
        .addProperty("width", "100%")
        .addProperty("max-width", "600px")
        .addProperty("margin", "0 auto")
        .addProperty("padding", "20px");
    css.createSelector("ul.list")
        .addProperty("list-style-type", "none")
        .addProperty("padding", "0");
    css.createSelector("li.item").addProperties(
        {{"padding", "10px 0"}, {"border-bottom", "1px solid #ddd"}});
    css.createSelector("li.item:last-child")
        .addProperty("border-bottom", "none");
    css.createSelector("li.item:hover").addProperty("background-color", "red");
    HtmlBuilder html{};
    html.head()
        .meta({{"charset", "UTF-8"}})
        .meta_()
        .meta({{"name", "viewport"},
               {"content", "width=device-width, initial-scale=1.0"}})
        .meta_()
        .title()
        .text("Hello World")
        .title_()
        .style()
        .text(css.toString())
        .style_()
        .head_();

    html.div({{"class", "container"}, {"id", "container"}})
        .ul({{"class", "list"}, {"id", "list"}});
    for (int i = 0; i < 10; i++)
        html.li({{"class", "item"}}).text("Item " + std::to_string(i)).li_();
    html.ul_().div_();
    html.script().text("console.log('Hello World');").script_();
    return res
        .set({
            {"Content-Type", "text/html"},
            {"Content-Length", std::to_string(html.toString().length())},
        })
        .status(200)
        .send(html.toString());
}

void post(const Request &req, Response &res) {
    std::cout << req.body.dump(2) << std::endl;
    return res.json({{"message", "Hello World"}});
}