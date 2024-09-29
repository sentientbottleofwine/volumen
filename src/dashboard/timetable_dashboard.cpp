#include "dashboard.hpp"
#include "../api.hpp"
#include "../timetable.hpp"
#include "../utils.hpp"
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/table.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/captured_mouse.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <spdlog/spdlog.h>

// TODO: implement caching 
ft::Component dashboard::timetable_dashboard::get_timetable_widget(api* api) {
    const std::string weekend_prompt = "No lessons today!";
    const std::shared_ptr<api::timetable_t> timetable_p = api->get_timetable();
    int today = utils::get_day_of_week(api->get_today());
    static int selector{};

    auto timetable_widget = ft::Container::Vertical({});

    int empty_counter{};
    for(const auto& lesson : *timetable_p->timetable[today]) {
        if(lesson.is_empty) {
            empty_counter++;
            continue;
        }
        timetable_widget->Add(ft::MenuEntry(lesson.subject));
    }

    if(empty_counter == timetable_p->timetable[today]->size())
        timetable_widget->Add(ft::MenuEntry(weekend_prompt));
    
    auto timeline_widget = timetable_dashboard::get_timeline_widget(timetable_p->timetable[today]);

    return ft::Renderer(timetable_widget, [=] {
        auto timetable = ft::hbox({
            timeline_widget,
            ft::separator(),
            timetable_widget->Render(),
        });
        if(timetable_widget->Focused()) 
            timetable = ft::window(ft::text("Timetable") | ft::hcenter, timetable | ft::color(ft::Color::White)) | ft::color(ft::Color::Green);
        else
            timetable = ft::window(ft::text("Timetable") | ft::hcenter, timetable );

        return timetable;
    });
    
}

ft::Element dashboard::timetable_dashboard::get_timeline_widget(std::shared_ptr<std::vector<api::lesson_t>> day) {
    const std::string deliminator = " - ";
    const std::string empty_placeholder = "-- - --";
    std::vector<ft::Element> entries;
    ft::Element el;

    int empty_counter{};

    for(const auto& lesson: *day) {
        if(lesson.is_empty) {
            empty_counter++;
            continue;
        }
        entries.push_back(ft::text(lesson.start + deliminator + lesson.end));
    }
    
    if(empty_counter == day->size())
        return ft::text(empty_placeholder);
    
    return ft::vbox({entries});
}