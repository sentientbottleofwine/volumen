#pragma once
#define NOMINMAX
#include <api/api.hpp>
#include <misc/config.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>

namespace ft = ftxui;

class custom_ui {
    // Hurr durr, you're holding state in a static class
    // Please show me a way to factor in the configuration without passing the config to every fucking method
    inline static const config* config_p = nullptr;
public:
    struct focus_management_t {
        bool active;
        bool focused;
    };
    static void init(const config* config); // Fuck clean code guidelines
    static ft::ButtonOption button_rounded();
    static ft::Component on_action(ft::Component component, std::function<void()> on_action);
    static ft::ComponentDecorator on_action(std::function<void()> on_action);
    static ft::Component custom_component_window(ft::Element title, ft::Component contents);
    static ft::Component custom_dropdown(ft::ConstStringListRef entries, int* selected);
    static ft::Component content_boxes(const std::vector<api::content_t*>& contents, int* selector, std::function<void()> on_select);
    static ft::Element focus_managed_window(ft::Element title, ft::Element contents, const focus_management_t& focus_management);
    static ft::Element focus_managed_border_box(ft::Element contents, const focus_management_t& focus_management);
    static ft::Element focus_managed_whatever(ft::Element contents, const focus_management_t& focus_management);
    static ft::InputOption plain_input();
    static int terminal_height();
};