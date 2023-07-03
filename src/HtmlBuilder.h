#ifndef HTML_BUILDER_H
#define HTML_BUILDER_H

#include <stack>
#include <string>
#include <unordered_map>

// Interface
/*
auto html = HtmlBuilder();
html.div({
    {"class", "container"},
    {"id", "container"}
})
    .ul({
        {"class", "list"},
        {"id", "list"}
    });

for (auto &item : items) {
    html.li({
        {"class", "item"},
        {"id", "item"}
    }).text(item).text_().li_();
}

html.ul_();

html.p({
    {"class", "paragraph"},
    {"id", "paragraph"}
}).text("Hello World").text_().p_();
html.div_();

std::string htmlString = html.toString();


})


*/

class HtmlBuilder {
   public:
    HtmlBuilder();
    ~HtmlBuilder();

    HtmlBuilder &a(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &a_();
    HtmlBuilder &abbr(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &abbr_();
    HtmlBuilder &address(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &address_();
    HtmlBuilder &area(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &area_();
    HtmlBuilder &article(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &article_();
    HtmlBuilder &aside(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &aside_();
    HtmlBuilder &audio(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &audio_();
    HtmlBuilder &b(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &b_();
    HtmlBuilder &base(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &base_();
    HtmlBuilder &bdi(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &bdi_();
    HtmlBuilder &bdo(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &bdo_();
    HtmlBuilder &blockquote(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &blockquote_();
    HtmlBuilder &body(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &body_();
    HtmlBuilder &br(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &br_();
    HtmlBuilder &button(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &button_();
    HtmlBuilder &canvas(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &canvas_();
    HtmlBuilder &caption(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &caption_();
    HtmlBuilder &cite(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &cite_();
    HtmlBuilder &code(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &code_();
    HtmlBuilder &col(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &col_();
    HtmlBuilder &colgroup(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &colgroup_();
    HtmlBuilder &data(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &data_();
    HtmlBuilder &datalist(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &datalist_();
    HtmlBuilder &dd(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &dd_();
    HtmlBuilder &del(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &del_();
    HtmlBuilder &details(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &details_();
    HtmlBuilder &dfn(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &dfn_();
    HtmlBuilder &dialog(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &dialog_();
    HtmlBuilder &div(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &div_();
    HtmlBuilder &dl(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &dl_();
    HtmlBuilder &dt(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &dt_();
    HtmlBuilder &em(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &em_();
    HtmlBuilder &embed(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &embed_();
    HtmlBuilder &fieldset(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &fieldset_();
    HtmlBuilder &figcaption(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &figcaption_();
    HtmlBuilder &figure(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &figure_();
    HtmlBuilder &footer(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &footer_();
    HtmlBuilder &form(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &form_();
    HtmlBuilder &h1(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &h1_();
    HtmlBuilder &h2(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &h2_();
    HtmlBuilder &h3(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &h3_();
    HtmlBuilder &h4(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &h4_();
    HtmlBuilder &h5(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &h5_();
    HtmlBuilder &h6(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &h6_();
    HtmlBuilder &head(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &head_();
    HtmlBuilder &header(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &header_();
    HtmlBuilder &hr(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &hr_();
    HtmlBuilder &html(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &html_();
    HtmlBuilder &i(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &i_();
    HtmlBuilder &iframe(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &iframe_();
    HtmlBuilder &img(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &img_();
    HtmlBuilder &input(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &input_();
    HtmlBuilder &ins(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &ins_();
    HtmlBuilder &kbd(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &kbd_();
    HtmlBuilder &label(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &label_();
    HtmlBuilder &legend(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &legend_();
    HtmlBuilder &li(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &li_();
    HtmlBuilder &link(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &link_();
    HtmlBuilder &main(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &main_();
    HtmlBuilder &map(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &map_();
    HtmlBuilder &mark(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &mark_();
    HtmlBuilder &meta(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &meta_();
    HtmlBuilder &meter(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &meter_();
    HtmlBuilder &nav(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &nav_();
    HtmlBuilder &noscript(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &noscript_();
    HtmlBuilder &object(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &object_();
    HtmlBuilder &ol(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &ol_();
    HtmlBuilder &optgroup(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &optgroup_();
    HtmlBuilder &option(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &option_();
    HtmlBuilder &output(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &output_();
    HtmlBuilder &p(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &p_();
    HtmlBuilder &param(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &param_();
    HtmlBuilder &picture(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &picture_();
    HtmlBuilder &pre(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &pre_();
    HtmlBuilder &progress(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &progress_();
    HtmlBuilder &q(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &q_();
    HtmlBuilder &rp(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &rp_();
    HtmlBuilder &rt(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &rt_();
    HtmlBuilder &ruby(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &ruby_();
    HtmlBuilder &s(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &s_();
    HtmlBuilder &samp(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &samp_();
    HtmlBuilder &script(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &script_();
    HtmlBuilder &section(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &section_();
    HtmlBuilder &select(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &select_();
    HtmlBuilder &small(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &small_();
    HtmlBuilder &source(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &source_();
    HtmlBuilder &span(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &span_();
    HtmlBuilder &strong(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &strong_();
    HtmlBuilder &style(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &style_();
    HtmlBuilder &sub(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &sub_();
    HtmlBuilder &summary(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &summary_();
    HtmlBuilder &sup(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &sup_();
    HtmlBuilder &svg(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &svg_();
    HtmlBuilder &table(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &table_();
    HtmlBuilder &tbody(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &tbody_();
    HtmlBuilder &td(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &td_();
    HtmlBuilder &template_(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &template_();
    HtmlBuilder &textarea(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &textarea_();
    HtmlBuilder &tfoot(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &tfoot_();
    HtmlBuilder &th(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &th_();
    HtmlBuilder &thead(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &thead_();
    HtmlBuilder &time(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &time_();
    HtmlBuilder &title(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &title_();
    HtmlBuilder &tr(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &tr_();
    HtmlBuilder &track(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &track_();
    HtmlBuilder &u(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &u_();
    HtmlBuilder &ul(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &ul_();
    HtmlBuilder &var(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &var_();
    HtmlBuilder &video(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &video_();
    HtmlBuilder &wbr(
        const std::unordered_map<std::string, std::string> &attributes = {});
    HtmlBuilder &wbr_();

    HtmlBuilder &text(const std::string &text);
    void end();
    std::string toString();

   private:
    std::string m_html;
    std::stack<std::string> m_tags;
};

#endif  // HTML_BUILDER_H