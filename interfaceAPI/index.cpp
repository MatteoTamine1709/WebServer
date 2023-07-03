#include <fstream>
#include <iostream>
#include <sstream>

#include "src/CssBuilder.h"
#include "src/HtmlBuilder.h"
#include "src/Request.h"
#include "src/Response.h"
#include "src/Sqlite.h"

extern "C" {
void get(const Request &req, Response &res);
void post(const Request &req, Response &res);
}

void get(const Request &req, Response &res) {
    // Sqlite db{};
    // std::cout << Sqlite::open("test.db") << std::endl;
    // // Sqlite::exec("DROP TABLE IF EXISTS users");
    // // Sqlite::exec(
    // //     "CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, name
    // TEXT,
    // //     " "age INTEGER)");
    // // Sqlite::insert("users", "name, age", "'John', 20");
    // // Sqlite::insert("users", "name, age", "'Jane', 21");
    // // Sqlite::insert("users", "name, age", "'Jack', 22");
    // CssBuilder css{};
    // css.createSelector("body")
    //     .addProperty("background-color", "#f1f1f1")
    //     .addProperty("font-family", "Verdana, sans-serif")
    //     .addProperty("font-size", "20px")
    //     .addProperty("line-height", "1.5")
    //     .addProperty("margin", "0")
    //     .addProperty("padding", "0")
    //     .addProperty("box-sizing", "border-box")
    //     .addProperty("display", "flex")
    //     .addProperty("justify-content", "center")
    //     .addProperty("align-items", "center")
    //     .addProperty("min-height", "100vh")
    //     .addProperty("color", "#666");
    // css.createSelector("div.container")
    //     .addProperty("width", "100%")
    //     .addProperty("max-width", "600px")
    //     .addProperty("margin", "0 auto")
    //     .addProperty("padding", "20px");
    // css.createSelector("ul.list")
    //     .addProperty("list-style-type", "none")
    //     .addProperty("padding", "0");
    // css.createSelector("li.item").addProperties(
    //     {{"padding", "10px 0"}, {"border-bottom", "1px solid #ddd"}});
    // css.createSelector("li.item:last-child")
    //     .addProperty("border-bottom", "none");
    // css.createSelector("li.item:hover").addProperty("background-color",
    // "red"); HtmlBuilder html{}; html.head()
    //     .meta({{"charset", "UTF-8"}})
    //     .meta_()
    //     .meta({{"name", "viewport"},
    //            {"content", "width=device-width, initial-scale=1.0"}})
    //     .meta_()
    //     .title()
    //     .text("Hello World")
    //     .title_()
    //     .style()
    //     .text(css.toString())
    //     .style_()
    //     .head_();
    // html.div({{"class", "container"}, {"id", "container"}})
    //     .ul({{"class", "list"}, {"id", "list"}});
    // auto lambda = [](void *data, int argc, char **argv,
    //                  char **azColName) -> int {
    //     HtmlBuilder *html = (HtmlBuilder *)data;
    //     html->li({{"class", "item"}})
    //         .text("id: " + std::string(argv[0]) + ", name: " + argv[1] +
    //               ", age: " + std::string(argv[2]))
    //         .li_();
    //     return 0;
    // };
    // Sqlite::select("users", "*", "id >= 1", lambda, &html);

    // html.ul_().div_();
    // html.script().text("console.log('Hello World');").script_();
    // res.set({
    //             {"Content-Type", "text/html"},
    //             {"Content-Length", std::to_string(html.toString().length())},
    //         })
    //     .status(200)
    //     .send(html.toString());
    res.sendFile("./app/dist/index.html");
}

void post(const Request &req, Response &res) {
    std::cout << req.body.dump(2) << std::endl;
    return res.json({{"message", "Hello World"}});
}