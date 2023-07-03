#include "../HtmlBuilder.h"

#include <stdexcept>

HtmlBuilder::HtmlBuilder() { m_html = "<!DOCTYPE html>"; }

HtmlBuilder::~HtmlBuilder() {}

HtmlBuilder &HtmlBuilder::a(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<a";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("a");
    return *this;
}

HtmlBuilder &HtmlBuilder::a_() {
    if (m_tags.empty() || m_tags.top() != "a")
        throw std::runtime_error("Invalid HTML");
    m_html += "</a>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::abbr(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<abbr";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("abbr");
    return *this;
}

HtmlBuilder &HtmlBuilder::abbr_() {
    if (m_tags.empty() || m_tags.top() != "abbr")
        throw std::runtime_error("Invalid HTML");
    m_html += "</abbr>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::address(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<address";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("address");
    return *this;
}

HtmlBuilder &HtmlBuilder::address_() {
    if (m_tags.empty() || m_tags.top() != "address")
        throw std::runtime_error("Invalid HTML");
    m_html += "</address>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::area(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<area";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("area");
    return *this;
}

HtmlBuilder &HtmlBuilder::area_() {
    if (m_tags.empty() || m_tags.top() != "area")
        throw std::runtime_error("Invalid HTML");
    m_html += "</area>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::article(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<article";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("article");
    return *this;
}

HtmlBuilder &HtmlBuilder::article_() {
    if (m_tags.empty() || m_tags.top() != "article")
        throw std::runtime_error("Invalid HTML");
    m_html += "</article>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::aside(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<aside";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("aside");
    return *this;
}

HtmlBuilder &HtmlBuilder::aside_() {
    if (m_tags.empty() || m_tags.top() != "aside")
        throw std::runtime_error("Invalid HTML");
    m_html += "</aside>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::audio(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<audio";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("audio");
    return *this;
}

HtmlBuilder &HtmlBuilder::audio_() {
    if (m_tags.empty() || m_tags.top() != "audio")
        throw std::runtime_error("Invalid HTML");
    m_html += "</audio>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::b(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<b";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("b");
    return *this;
}

HtmlBuilder &HtmlBuilder::b_() {
    if (m_tags.empty() || m_tags.top() != "b")
        throw std::runtime_error("Invalid HTML");
    m_html += "</b>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::base(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<base";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("base");
    return *this;
}

HtmlBuilder &HtmlBuilder::base_() {
    if (m_tags.empty() || m_tags.top() != "base")
        throw std::runtime_error("Invalid HTML");
    m_html += "</base>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::bdi(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<bdi";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("bdi");
    return *this;
}

HtmlBuilder &HtmlBuilder::bdi_() {
    if (m_tags.empty() || m_tags.top() != "bdi")
        throw std::runtime_error("Invalid HTML");
    m_html += "</bdi>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::bdo(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<bdo";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("bdo");
    return *this;
}

HtmlBuilder &HtmlBuilder::bdo_() {
    if (m_tags.empty() || m_tags.top() != "bdo")
        throw std::runtime_error("Invalid HTML");
    m_html += "</bdo>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::blockquote(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<blockquote";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("blockquote");
    return *this;
}

HtmlBuilder &HtmlBuilder::blockquote_() {
    if (m_tags.empty() || m_tags.top() != "blockquote")
        throw std::runtime_error("Invalid HTML");
    m_html += "</blockquote>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::body(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<body";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("body");
    return *this;
}

HtmlBuilder &HtmlBuilder::body_() {
    if (m_tags.empty() || m_tags.top() != "body")
        throw std::runtime_error("Invalid HTML");
    m_html += "</body>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::br(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<br";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("br");
    return *this;
}

HtmlBuilder &HtmlBuilder::br_() {
    if (m_tags.empty() || m_tags.top() != "br")
        throw std::runtime_error("Invalid HTML");
    m_html += "</br>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::button(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<button";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("button");
    return *this;
}

HtmlBuilder &HtmlBuilder::button_() {
    if (m_tags.empty() || m_tags.top() != "button")
        throw std::runtime_error("Invalid HTML");
    m_html += "</button>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::canvas(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<canvas";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("canvas");
    return *this;
}

HtmlBuilder &HtmlBuilder::canvas_() {
    if (m_tags.empty() || m_tags.top() != "canvas")
        throw std::runtime_error("Invalid HTML");
    m_html += "</canvas>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::caption(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<caption";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("caption");
    return *this;
}

HtmlBuilder &HtmlBuilder::caption_() {
    if (m_tags.empty() || m_tags.top() != "caption")
        throw std::runtime_error("Invalid HTML");
    m_html += "</caption>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::cite(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<cite";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("cite");
    return *this;
}

HtmlBuilder &HtmlBuilder::cite_() {
    if (m_tags.empty() || m_tags.top() != "cite")
        throw std::runtime_error("Invalid HTML");
    m_html += "</cite>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::code(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<code";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("code");
    return *this;
}

HtmlBuilder &HtmlBuilder::code_() {
    if (m_tags.empty() || m_tags.top() != "code")
        throw std::runtime_error("Invalid HTML");
    m_html += "</code>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::col(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<col";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("col");
    return *this;
}

HtmlBuilder &HtmlBuilder::col_() {
    if (m_tags.empty() || m_tags.top() != "col")
        throw std::runtime_error("Invalid HTML");
    m_html += "</col>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::colgroup(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<colgroup";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("colgroup");
    return *this;
}

HtmlBuilder &HtmlBuilder::colgroup_() {
    if (m_tags.empty() || m_tags.top() != "colgroup")
        throw std::runtime_error("Invalid HTML");
    m_html += "</colgroup>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::data(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<data";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("data");
    return *this;
}

HtmlBuilder &HtmlBuilder::data_() {
    if (m_tags.empty() || m_tags.top() != "data")
        throw std::runtime_error("Invalid HTML");
    m_html += "</data>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::datalist(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<datalist";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("datalist");
    return *this;
}

HtmlBuilder &HtmlBuilder::datalist_() {
    if (m_tags.empty() || m_tags.top() != "datalist")
        throw std::runtime_error("Invalid HTML");
    m_html += "</datalist>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::dd(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<dd";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("dd");
    return *this;
}

HtmlBuilder &HtmlBuilder::dd_() {
    if (m_tags.empty() || m_tags.top() != "dd")
        throw std::runtime_error("Invalid HTML");
    m_html += "</dd>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::del(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<del";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("del");
    return *this;
}

HtmlBuilder &HtmlBuilder::del_() {
    if (m_tags.empty() || m_tags.top() != "del")
        throw std::runtime_error("Invalid HTML");
    m_html += "</del>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::details(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<details";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("details");
    return *this;
}

HtmlBuilder &HtmlBuilder::details_() {
    if (m_tags.empty() || m_tags.top() != "details")
        throw std::runtime_error("Invalid HTML");
    m_html += "</details>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::dfn(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<dfn";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("dfn");
    return *this;
}

HtmlBuilder &HtmlBuilder::dfn_() {
    if (m_tags.empty() || m_tags.top() != "dfn")
        throw std::runtime_error("Invalid HTML");
    m_html += "</dfn>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::dialog(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<dialog";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("dialog");
    return *this;
}

HtmlBuilder &HtmlBuilder::dialog_() {
    if (m_tags.empty() || m_tags.top() != "dialog")
        throw std::runtime_error("Invalid HTML");
    m_html += "</dialog>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::div(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<div";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("div");
    return *this;
}

HtmlBuilder &HtmlBuilder::div_() {
    if (m_tags.empty() || m_tags.top() != "div")
        throw std::runtime_error("Invalid HTML");
    m_html += "</div>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::dl(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<dl";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("dl");
    return *this;
}

HtmlBuilder &HtmlBuilder::dl_() {
    if (m_tags.empty() || m_tags.top() != "dl")
        throw std::runtime_error("Invalid HTML");
    m_html += "</dl>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::dt(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<dt";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("dt");
    return *this;
}

HtmlBuilder &HtmlBuilder::dt_() {
    if (m_tags.empty() || m_tags.top() != "dt")
        throw std::runtime_error("Invalid HTML");
    m_html += "</dt>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::em(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<em";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("em");
    return *this;
}

HtmlBuilder &HtmlBuilder::em_() {
    if (m_tags.empty() || m_tags.top() != "em")
        throw std::runtime_error("Invalid HTML");
    m_html += "</em>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::embed(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<embed";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("embed");
    return *this;
}

HtmlBuilder &HtmlBuilder::embed_() {
    if (m_tags.empty() || m_tags.top() != "embed")
        throw std::runtime_error("Invalid HTML");
    m_html += "</embed>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::fieldset(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<fieldset";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("fieldset");
    return *this;
}

HtmlBuilder &HtmlBuilder::fieldset_() {
    if (m_tags.empty() || m_tags.top() != "fieldset")
        throw std::runtime_error("Invalid HTML");
    m_html += "</fieldset>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::figcaption(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<figcaption";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("figcaption");
    return *this;
}

HtmlBuilder &HtmlBuilder::figcaption_() {
    if (m_tags.empty() || m_tags.top() != "figcaption")
        throw std::runtime_error("Invalid HTML");
    m_html += "</figcaption>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::figure(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<figure";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("figure");
    return *this;
}

HtmlBuilder &HtmlBuilder::figure_() {
    if (m_tags.empty() || m_tags.top() != "figure")
        throw std::runtime_error("Invalid HTML");
    m_html += "</figure>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::footer(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<footer";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("footer");
    return *this;
}

HtmlBuilder &HtmlBuilder::footer_() {
    if (m_tags.empty() || m_tags.top() != "footer")
        throw std::runtime_error("Invalid HTML");
    m_html += "</footer>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::form(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<form";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("form");
    return *this;
}

HtmlBuilder &HtmlBuilder::form_() {
    if (m_tags.empty() || m_tags.top() != "form")
        throw std::runtime_error("Invalid HTML");
    m_html += "</form>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::h1(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<h1";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("h1");
    return *this;
}

HtmlBuilder &HtmlBuilder::h1_() {
    if (m_tags.empty() || m_tags.top() != "h1")
        throw std::runtime_error("Invalid HTML");
    m_html += "</h1>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::h2(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<h2";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("h2");
    return *this;
}

HtmlBuilder &HtmlBuilder::h2_() {
    if (m_tags.empty() || m_tags.top() != "h2")
        throw std::runtime_error("Invalid HTML");
    m_html += "</h2>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::h3(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<h3";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("h3");
    return *this;
}

HtmlBuilder &HtmlBuilder::h3_() {
    if (m_tags.empty() || m_tags.top() != "h3")
        throw std::runtime_error("Invalid HTML");
    m_html += "</h3>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::h4(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<h4";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("h4");
    return *this;
}

HtmlBuilder &HtmlBuilder::h4_() {
    if (m_tags.empty() || m_tags.top() != "h4")
        throw std::runtime_error("Invalid HTML");
    m_html += "</h4>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::h5(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<h5";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("h5");
    return *this;
}

HtmlBuilder &HtmlBuilder::h5_() {
    if (m_tags.empty() || m_tags.top() != "h5")
        throw std::runtime_error("Invalid HTML");
    m_html += "</h5>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::h6(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<h6";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("h6");
    return *this;
}

HtmlBuilder &HtmlBuilder::h6_() {
    if (m_tags.empty() || m_tags.top() != "h6")
        throw std::runtime_error("Invalid HTML");
    m_html += "</h6>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::head(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<head";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("head");
    return *this;
}

HtmlBuilder &HtmlBuilder::head_() {
    if (m_tags.empty() || m_tags.top() != "head")
        throw std::runtime_error("Invalid HTML");
    m_html += "</head>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::header(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<header";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("header");
    return *this;
}

HtmlBuilder &HtmlBuilder::header_() {
    if (m_tags.empty() || m_tags.top() != "header")
        throw std::runtime_error("Invalid HTML");
    m_html += "</header>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::hr(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<hr";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("hr");
    return *this;
}

HtmlBuilder &HtmlBuilder::hr_() {
    if (m_tags.empty() || m_tags.top() != "hr")
        throw std::runtime_error("Invalid HTML");
    m_html += "</hr>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::html(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<html";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("html");
    return *this;
}

HtmlBuilder &HtmlBuilder::html_() {
    if (m_tags.empty() || m_tags.top() != "html")
        throw std::runtime_error("Invalid HTML");
    m_html += "</html>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::i(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<i";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("i");
    return *this;
}

HtmlBuilder &HtmlBuilder::i_() {
    if (m_tags.empty() || m_tags.top() != "i")
        throw std::runtime_error("Invalid HTML");
    m_html += "</i>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::iframe(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<iframe";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("iframe");
    return *this;
}

HtmlBuilder &HtmlBuilder::iframe_() {
    if (m_tags.empty() || m_tags.top() != "iframe")
        throw std::runtime_error("Invalid HTML");
    m_html += "</iframe>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::img(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<img";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("img");
    return *this;
}

HtmlBuilder &HtmlBuilder::img_() {
    if (m_tags.empty() || m_tags.top() != "img")
        throw std::runtime_error("Invalid HTML");
    m_html += "</img>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::input(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<input";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    return *this;
}

HtmlBuilder &HtmlBuilder::ins(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<ins";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("ins");
    return *this;
}

HtmlBuilder &HtmlBuilder::ins_() {
    if (m_tags.empty() || m_tags.top() != "ins")
        throw std::runtime_error("Invalid HTML");
    m_html += "</ins>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::kbd(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<kbd";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("kbd");
    return *this;
}

HtmlBuilder &HtmlBuilder::kbd_() {
    if (m_tags.empty() || m_tags.top() != "kbd")
        throw std::runtime_error("Invalid HTML");
    m_html += "</kbd>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::label(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<label";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("label");
    return *this;
}

HtmlBuilder &HtmlBuilder::label_() {
    if (m_tags.empty() || m_tags.top() != "label")
        throw std::runtime_error("Invalid HTML");
    m_html += "</label>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::legend(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<legend";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("legend");
    return *this;
}

HtmlBuilder &HtmlBuilder::legend_() {
    if (m_tags.empty() || m_tags.top() != "legend")
        throw std::runtime_error("Invalid HTML");
    m_html += "</legend>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::li(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<li";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("li");
    return *this;
}

HtmlBuilder &HtmlBuilder::li_() {
    if (m_tags.empty() || m_tags.top() != "li")
        throw std::runtime_error("Invalid HTML");
    m_html += "</li>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::link(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<link";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    return *this;
}

HtmlBuilder &HtmlBuilder::link_() {
    if (m_tags.empty() || m_tags.top() != "link")
        throw std::runtime_error("Invalid HTML");
    m_html += "</link>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::main(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<main";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("main");
    return *this;
}

HtmlBuilder &HtmlBuilder::main_() {
    if (m_tags.empty() || m_tags.top() != "main")
        throw std::runtime_error("Invalid HTML");
    m_html += "</main>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::map(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<map";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("map");
    return *this;
}

HtmlBuilder &HtmlBuilder::map_() {
    if (m_tags.empty() || m_tags.top() != "map")
        throw std::runtime_error("Invalid HTML");
    m_html += "</map>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::mark(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<mark";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("mark");
    return *this;
}

HtmlBuilder &HtmlBuilder::mark_() {
    if (m_tags.empty() || m_tags.top() != "mark")
        throw std::runtime_error("Invalid HTML");
    m_html += "</mark>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::meta(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<meta";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("meta");
    return *this;
}

HtmlBuilder &HtmlBuilder::meta_() {
    if (m_tags.empty() || m_tags.top() != "meta")
        throw std::runtime_error("Invalid HTML");
    m_html += "</meta>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::meter(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<meter";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("meter");
    return *this;
}

HtmlBuilder &HtmlBuilder::meter_() {
    if (m_tags.empty() || m_tags.top() != "meter")
        throw std::runtime_error("Invalid HTML");
    m_html += "</meter>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::nav(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<nav";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("nav");
    return *this;
}

HtmlBuilder &HtmlBuilder::nav_() {
    if (m_tags.empty() || m_tags.top() != "nav")
        throw std::runtime_error("Invalid HTML");
    m_html += "</nav>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::noscript(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<noscript";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("noscript");
    return *this;
}

HtmlBuilder &HtmlBuilder::noscript_() {
    if (m_tags.empty() || m_tags.top() != "noscript")
        throw std::runtime_error("Invalid HTML");
    m_html += "</noscript>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::object(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<object";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("object");
    return *this;
}

HtmlBuilder &HtmlBuilder::object_() {
    if (m_tags.empty() || m_tags.top() != "object")
        throw std::runtime_error("Invalid HTML");
    m_html += "</object>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::ol(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<ol";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("ol");
    return *this;
}

HtmlBuilder &HtmlBuilder::ol_() {
    if (m_tags.empty() || m_tags.top() != "ol")
        throw std::runtime_error("Invalid HTML");
    m_html += "</ol>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::optgroup(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<optgroup";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("optgroup");
    return *this;
}

HtmlBuilder &HtmlBuilder::optgroup_() {
    if (m_tags.empty() || m_tags.top() != "optgroup")
        throw std::runtime_error("Invalid HTML");
    m_html += "</optgroup>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::option(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<option";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("option");
    return *this;
}

HtmlBuilder &HtmlBuilder::option_() {
    if (m_tags.empty() || m_tags.top() != "option")
        throw std::runtime_error("Invalid HTML");
    m_html += "</option>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::output(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<output";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("output");
    return *this;
}

HtmlBuilder &HtmlBuilder::output_() {
    if (m_tags.empty() || m_tags.top() != "output")
        throw std::runtime_error("Invalid HTML");
    m_html += "</output>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::p(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<p";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("p");
    return *this;
}

HtmlBuilder &HtmlBuilder::p_() {
    if (m_tags.empty() || m_tags.top() != "p")
        throw std::runtime_error("Invalid HTML");
    m_html += "</p>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::param(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<param";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("param");
    return *this;
}

HtmlBuilder &HtmlBuilder::param_() {
    if (m_tags.empty() || m_tags.top() != "param")
        throw std::runtime_error("Invalid HTML");
    m_html += "</param>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::picture(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<picture";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("picture");
    return *this;
}

HtmlBuilder &HtmlBuilder::picture_() {
    if (m_tags.empty() || m_tags.top() != "picture")
        throw std::runtime_error("Invalid HTML");
    m_html += "</picture>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::pre(
    const std::unordered_map<std::string, std::string> &attributes) {
    m_html += "<pre";
    for (auto &attribute : attributes)
        m_html += " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += ">";
    m_tags.push("pre");
    return *this;
}

HtmlBuilder &HtmlBuilder::pre_() {
    if (m_tags.empty() || m_tags.top() != "pre")
        throw std::runtime_error("Invalid HTML");
    m_html += "</pre>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::progress(
    const std::unordered_map<std::string, std::string> &attributes) {
    std::string attributesString = "";
    for (auto &attribute : attributes) {
        attributesString +=
            " " + attribute.first + "=\"" + attribute.second + "\"";
    }
    m_html += "<progress" + attributesString + ">";
    m_tags.push("progress");
    return *this;
}

HtmlBuilder &HtmlBuilder::progress_() {
    if (m_tags.empty() || m_tags.top() != "progress")
        throw std::runtime_error("Invalid HTML");
    m_html += "</progress>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::q(
    const std::unordered_map<std::string, std::string> &attributes) {
    std::string attributesString = "";
    for (auto &attribute : attributes) {
        attributesString +=
            " " + attribute.first + "=\"" + attribute.second + "\"";
    }
    m_html += "<q" + attributesString + ">";
    m_tags.push("q");
    return *this;
}

HtmlBuilder &HtmlBuilder::q_() {
    if (m_tags.empty() || m_tags.top() != "q")
        throw std::runtime_error("Invalid HTML");
    m_html += "</q>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::rp(
    const std::unordered_map<std::string, std::string> &attributes) {
    std::string attributesString = "";
    for (auto &attribute : attributes) {
        attributesString +=
            " " + attribute.first + "=\"" + attribute.second + "\"";
    }
    m_html += "<rp" + attributesString + ">";
    m_tags.push("rp");
    return *this;
}

HtmlBuilder &HtmlBuilder::rp_() {
    if (m_tags.empty() || m_tags.top() != "rp")
        throw std::runtime_error("Invalid HTML");
    m_html += "</rp>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::rt(
    const std::unordered_map<std::string, std::string> &attributes) {
    std::string attributesString = "";
    for (auto &attribute : attributes) {
        attributesString +=
            " " + attribute.first + "=\"" + attribute.second + "\"";
    }
    m_html += "<rt" + attributesString + ">";
    m_tags.push("rt");
    return *this;
}

HtmlBuilder &HtmlBuilder::rt_() {
    if (m_tags.empty() || m_tags.top() != "rt")
        throw std::runtime_error("Invalid HTML");
    m_html += "</rt>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::ruby(
    const std::unordered_map<std::string, std::string> &attributes) {
    std::string attributesString = "";
    for (auto &attribute : attributes) {
        attributesString +=
            " " + attribute.first + "=\"" + attribute.second + "\"";
    }
    m_html += "<ruby" + attributesString + ">";
    m_tags.push("ruby");
    return *this;
}

HtmlBuilder &HtmlBuilder::ruby_() {
    if (m_tags.empty() || m_tags.top() != "ruby")
        throw std::runtime_error("Invalid HTML");
    m_html += "</ruby>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::s(
    const std::unordered_map<std::string, std::string> &attributes) {
    std::string attributesString = "";
    for (auto &attribute : attributes) {
        attributesString +=
            " " + attribute.first + "=\"" + attribute.second + "\"";
    }
    m_html += "<s" + attributesString + ">";
    m_tags.push("s");
    return *this;
}

HtmlBuilder &HtmlBuilder::s_() {
    if (m_tags.empty() || m_tags.top() != "s")
        throw std::runtime_error("Invalid HTML");
    m_html += "</s>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::samp(
    const std::unordered_map<std::string, std::string> &attributes) {
    std::string attributesString = "";
    for (auto &attribute : attributes) {
        attributesString +=
            " " + attribute.first + "=\"" + attribute.second + "\"";
    }
    m_html += "<samp" + attributesString + ">";
    m_tags.push("samp");
    return *this;
}

HtmlBuilder &HtmlBuilder::samp_() {
    if (m_tags.empty() || m_tags.top() != "samp")
        throw std::runtime_error("Invalid HTML");
    m_html += "</samp>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::script(
    const std::unordered_map<std::string, std::string> &attributes) {
    std::string attributesString = "";
    for (auto &attribute : attributes) {
        attributesString +=
            " " + attribute.first + "=\"" + attribute.second + "\"";
    }
    m_html += "<script" + attributesString + ">";
    m_tags.push("script");
    return *this;
}

HtmlBuilder &HtmlBuilder::script_() {
    if (m_tags.empty() || m_tags.top() != "script")
        throw std::runtime_error("Invalid HTML");
    m_html += "</script>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::section(
    const std::unordered_map<std::string, std::string> &attributes) {
    std::string attributesString = "";
    for (const auto &attribute : attributes) {
        attributesString +=
            " " + attribute.first + "=\"" + attribute.second + "\"";
    }
    m_html += "<section" + attributesString + ">";
    m_tags.push("section");
    return *this;
}

HtmlBuilder &HtmlBuilder::section_() {
    if (m_tags.empty() || m_tags.top() != "section")
        throw std::runtime_error("Invalid HTML");
    m_html += "</section>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::select(
    const std::unordered_map<std::string, std::string> &attributes) {
    std::string attributesString = "";
    for (const auto &attribute : attributes) {
        attributesString +=
            " " + attribute.first + "=\"" + attribute.second + "\"";
    }
    m_html += "<select" + attributesString + ">";
    m_tags.push("select");
    return *this;
}

HtmlBuilder &HtmlBuilder::select_() {
    if (m_tags.empty() || m_tags.top() != "select")
        throw std::runtime_error("Invalid HTML");
    m_html += "</select>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::small(
    const std::unordered_map<std::string, std::string> &attributes) {
    std::string attributesString = "";
    for (const auto &attribute : attributes) {
        attributesString +=
            " " + attribute.first + "=\"" + attribute.second + "\"";
    }
    m_html += "<small" + attributesString + ">";
    m_tags.push("small");
    return *this;
}

HtmlBuilder &HtmlBuilder::small_() {
    if (m_tags.empty() || m_tags.top() != "small")
        throw std::runtime_error("Invalid HTML");
    m_html += "</small>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::source(
    const std::unordered_map<std::string, std::string> &attributes) {
    std::string attributesString = "";
    for (const auto &attribute : attributes) {
        attributesString +=
            " " + attribute.first + "=\"" + attribute.second + "\"";
    }
    m_html += "<source" + attributesString + ">";
    m_tags.push("source");
    return *this;
}

HtmlBuilder &HtmlBuilder::source_() {
    if (m_tags.empty() || m_tags.top() != "source")
        throw std::runtime_error("Invalid HTML");
    m_html += "</source>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::span(
    const std::unordered_map<std::string, std::string> &attributes) {
    std::string attributesString = "";
    for (const auto &attribute : attributes) {
        attributesString +=
            " " + attribute.first + "=\"" + attribute.second + "\"";
    }
    m_html += "<span" + attributesString + ">";
    m_tags.push("span");
    return *this;
}

HtmlBuilder &HtmlBuilder::span_() {
    if (m_tags.empty() || m_tags.top() != "span")
        throw std::runtime_error("Invalid HTML");
    m_html += "</span>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::strong(
    const std::unordered_map<std::string, std::string> &attributes) {
    std::string attributesString = "";
    for (const auto &attribute : attributes) {
        attributesString +=
            " " + attribute.first + "=\"" + attribute.second + "\"";
    }
    m_html += "<strong" + attributesString + ">";
    m_tags.push("strong");
    return *this;
}

HtmlBuilder &HtmlBuilder::strong_() {
    if (m_tags.empty() || m_tags.top() != "strong")
        throw std::runtime_error("Invalid HTML");
    m_html += "</strong>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::style(
    const std::unordered_map<std::string, std::string> &attributes) {
    std::string attributesString = "";
    for (const auto &attribute : attributes) {
        attributesString +=
            " " + attribute.first + "=\"" + attribute.second + "\"";
    }
    m_html += "<style" + attributesString + ">";
    m_tags.push("style");
    return *this;
}

HtmlBuilder &HtmlBuilder::style_() {
    if (m_tags.empty() || m_tags.top() != "style")
        throw std::runtime_error("Invalid HTML");
    m_html += "</style>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::sub(
    const std::unordered_map<std::string, std::string> &attributes) {
    std::string attributesString = "";
    for (const auto &attribute : attributes) {
        attributesString +=
            " " + attribute.first + "=\"" + attribute.second + "\"";
    }
    m_html += "<sub" + attributesString + ">";
    m_tags.push("sub");
    return *this;
}

HtmlBuilder &HtmlBuilder::sub_() {
    if (m_tags.empty() || m_tags.top() != "sub")
        throw std::runtime_error("Invalid HTML");
    m_html += "</sub>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::summary(
    const std::unordered_map<std::string, std::string> &attributes) {
    std::string attributesString = "";
    for (const auto &attribute : attributes) {
        attributesString +=
            " " + attribute.first + "=\"" + attribute.second + "\"";
    }
    m_html += "<summary" + attributesString + ">";
    m_tags.push("summary");
    return *this;
}

HtmlBuilder &HtmlBuilder::summary_() {
    if (m_tags.empty() || m_tags.top() != "summary")
        throw std::runtime_error("Invalid HTML");
    m_html += "</summary>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::sup(
    const std::unordered_map<std::string, std::string> &attributes) {
    std::string attributesString = "";
    for (const auto &attribute : attributes)
        attributesString +=
            " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += "<sup" + attributesString + ">";
    m_tags.push("sup");
    return *this;
}

HtmlBuilder &HtmlBuilder::sup_() {
    if (m_tags.empty() || m_tags.top() != "sup")
        throw std::runtime_error("Invalid HTML");
    m_html += "</sup>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::svg(
    const std::unordered_map<std::string, std::string> &attributes) {
    std::string attributesString = "";
    for (const auto &attribute : attributes)
        attributesString +=
            " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += "<svg" + attributesString + ">";
    m_tags.push("svg");
    return *this;
}

HtmlBuilder &HtmlBuilder::svg_() {
    if (m_tags.empty() || m_tags.top() != "svg")
        throw std::runtime_error("Invalid HTML");
    m_html += "</svg>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::table(
    const std::unordered_map<std::string, std::string> &attributes) {
    std::string attributesString = "";
    for (const auto &attribute : attributes)
        attributesString +=
            " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += "<table" + attributesString + ">";
    m_tags.push("table");
    return *this;
}

HtmlBuilder &HtmlBuilder::table_() {
    if (m_tags.empty() || m_tags.top() != "table")
        throw std::runtime_error("Invalid HTML");
    m_html += "</table>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::tbody(
    const std::unordered_map<std::string, std::string> &attributes) {
    std::string attributesString = "";
    for (const auto &attribute : attributes)
        attributesString +=
            " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += "<tbody" + attributesString + ">";
    m_tags.push("tbody");
    return *this;
}

HtmlBuilder &HtmlBuilder::tbody_() {
    if (m_tags.empty() || m_tags.top() != "tbody")
        throw std::runtime_error("Invalid HTML");
    m_html += "</tbody>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::td(
    const std::unordered_map<std::string, std::string> &attributes) {
    std::string attributesString = "";
    for (const auto &attribute : attributes)
        attributesString +=
            " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += "<td" + attributesString + ">";
    m_tags.push("td");
    return *this;
}

HtmlBuilder &HtmlBuilder::td_() {
    if (m_tags.empty() || m_tags.top() != "td")
        throw std::runtime_error("Invalid HTML");
    m_html += "</td>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::template_(
    const std::unordered_map<std::string, std::string> &attributes) {
    std::string attributesString = "";
    for (const auto &attribute : attributes)
        attributesString +=
            " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += "<template" + attributesString + ">";
    m_tags.push("template");
    return *this;
}

HtmlBuilder &HtmlBuilder::template_() {
    if (m_tags.empty() || m_tags.top() != "template")
        throw std::runtime_error("Invalid HTML");
    m_html += "</template>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::textarea(
    const std::unordered_map<std::string, std::string> &attributes) {
    std::string attributesString = "";
    for (const auto &attribute : attributes)
        attributesString +=
            " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += "<textarea" + attributesString + ">";
    m_tags.push("textarea");
    return *this;
}

HtmlBuilder &HtmlBuilder::textarea_() {
    if (m_tags.empty() || m_tags.top() != "textarea")
        throw std::runtime_error("Invalid HTML");
    m_html += "</textarea>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::tfoot(
    const std::unordered_map<std::string, std::string> &attributes) {
    std::string attributesString = "";
    for (const auto &attribute : attributes)
        attributesString +=
            " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += "<tfoot" + attributesString + ">";
    m_tags.push("tfoot");
    return *this;
}

HtmlBuilder &HtmlBuilder::tfoot_() {
    if (m_tags.empty() || m_tags.top() != "tfoot")
        throw std::runtime_error("Invalid HTML");
    m_html += "</tfoot>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::th(
    const std::unordered_map<std::string, std::string> &attributes) {
    std::string attributesString = "";
    for (const auto &attribute : attributes)
        attributesString +=
            " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += "<th" + attributesString + ">";
    m_tags.push("th");
    return *this;
}

HtmlBuilder &HtmlBuilder::th_() {
    if (m_tags.empty() || m_tags.top() != "th")
        throw std::runtime_error("Invalid HTML");
    m_html += "</th>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::thead(
    const std::unordered_map<std::string, std::string> &attributes) {
    std::string attributesString = "";
    for (const auto &attribute : attributes)
        attributesString +=
            " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += "<thead" + attributesString + ">";
    m_tags.push("thead");
    return *this;
}

HtmlBuilder &HtmlBuilder::thead_() {
    if (m_tags.empty() || m_tags.top() != "thead")
        throw std::runtime_error("Invalid HTML");
    m_html += "</thead>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::time(
    const std::unordered_map<std::string, std::string> &attributes) {
    std::string attributesString = "";
    for (const auto &attribute : attributes)
        attributesString +=
            " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += "<time" + attributesString + ">";
    m_tags.push("time");
    return *this;
}

HtmlBuilder &HtmlBuilder::time_() {
    if (m_tags.empty() || m_tags.top() != "time")
        throw std::runtime_error("Invalid HTML");
    m_html += "</time>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::title(
    const std::unordered_map<std::string, std::string> &attributes) {
    std::string attributesString = "";
    for (const auto &attribute : attributes)
        attributesString +=
            " " + attribute.first + "=\"" + attribute.second + "\"";

    m_html += "<title" + attributesString + ">";
    m_tags.push("title");
    return *this;
}

HtmlBuilder &HtmlBuilder::title_() {
    if (m_tags.empty() || m_tags.top() != "title")
        throw std::runtime_error("Invalid HTML");
    m_html += "</title>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::tr(
    const std::unordered_map<std::string, std::string> &attributes) {
    std::string attributesString = "";
    for (const auto &attribute : attributes)
        attributesString +=
            " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += "<tr" + attributesString + ">";
    m_tags.push("tr");
    return *this;
}

HtmlBuilder &HtmlBuilder::tr_() {
    if (m_tags.empty() || m_tags.top() != "tr")
        throw std::runtime_error("Invalid HTML");
    m_html += "</tr>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::track(
    const std::unordered_map<std::string, std::string> &attributes) {
    std::string attributesString = "";
    for (const auto &attribute : attributes)
        attributesString +=
            " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += "<track" + attributesString + ">";
    m_tags.push("track");
    return *this;
}

HtmlBuilder &HtmlBuilder::track_() {
    if (m_tags.empty() || m_tags.top() != "track")
        throw std::runtime_error("Invalid HTML");
    m_html += "</track>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::u(
    const std::unordered_map<std::string, std::string> &attributes) {
    std::string attributesString = "";
    for (const auto &attribute : attributes)
        attributesString +=
            " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += "<u" + attributesString + ">";
    m_tags.push("u");
    return *this;
}

HtmlBuilder &HtmlBuilder::u_() {
    if (m_tags.empty() || m_tags.top() != "u")
        throw std::runtime_error("Invalid HTML");
    m_html += "</u>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::ul(
    const std::unordered_map<std::string, std::string> &attributes) {
    std::string attributesString = "";
    for (const auto &attribute : attributes)
        attributesString +=
            " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += "<ul" + attributesString + ">";
    m_tags.push("ul");
    return *this;
}

HtmlBuilder &HtmlBuilder::ul_() {
    if (m_tags.empty() || m_tags.top() != "ul")
        throw std::runtime_error("Invalid HTML");
    m_html += "</ul>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::var(
    const std::unordered_map<std::string, std::string> &attributes) {
    std::string attributesString = "";
    for (const auto &attribute : attributes)
        attributesString +=
            " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += "<var" + attributesString + ">";
    m_tags.push("var");
    return *this;
}

HtmlBuilder &HtmlBuilder::var_() {
    if (m_tags.empty() || m_tags.top() != "var")
        throw std::runtime_error("Invalid HTML");
    m_html += "</var>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::video(
    const std::unordered_map<std::string, std::string> &attributes) {
    std::string attributesString = "";
    for (const auto &attribute : attributes)
        attributesString +=
            " " + attribute.first + "=\"" + attribute.second + "\"";
    m_html += "<video" + attributesString + ">";
    m_tags.push("video");
    return *this;
}

HtmlBuilder &HtmlBuilder::video_() {
    if (m_tags.empty() || m_tags.top() != "video")
        throw std::runtime_error("Invalid HTML");
    m_html += "</video>";
    m_tags.pop();
    return *this;
}

HtmlBuilder &HtmlBuilder::wbr(
    const std::unordered_map<std::string, std::string> &attributes) {
    std::string attributesString = "";
    for (const auto &attribute : attributes) {
        attributesString +=
            " " + attribute.first + "=\"" + attribute.second + "\"";
    }
    m_html += "<wbr" + attributesString + ">";
    m_tags.push("wbr");
    return *this;
}

HtmlBuilder &HtmlBuilder::wbr_() {
    m_html += "</wbr>";
    return *this;
}

HtmlBuilder &HtmlBuilder::text(const std::string &text) {
    m_html += text;
    return *this;
}

void HtmlBuilder::end() {
    while (!m_tags.empty()) {
        m_html += "</" + m_tags.top() + ">";
        m_tags.pop();
    }
}

std::string HtmlBuilder::toString() {
    end();
    return m_html;
}
