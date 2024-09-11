#pragma once
#include "content.hpp"
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>
#include <string>
#include <vector>

namespace ft = ftxui;

class messages : public content {
    size_t selected{};
    std::shared_ptr<api::messages_t> messages_p;
public:
    messages(ftxui::ScreenInteractive* main_screen);
    void content_display(ft::Component content_component, api* api, size_t* redirect, std::mutex* redirect_mutex) override;
    ft::Component content_view() override;
};