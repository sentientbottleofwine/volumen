#include <ui/messages.hpp>
#include <ui/custom_ui.hpp>
#include <ui/main_ui.hpp>
#include <misc/utils.hpp>
#include <string>
#include <vector>
#include <chrono>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/table.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/captured_mouse.hpp>
#include <ftxui/component/screen_interactive.hpp>

ft::Component messages::content_view() {
    auto message = (messages_type_selected_ ? messages_.sent : messages_.recieved).at(message_selected_);
    const std::string deliminator = " | ";
    const std::string quit_message = "Press q or Ctrl+C to quit";
    const int DATE_SIZE = 10;
    auto date_timepoint = std::chrono::sys_seconds(std::chrono::seconds(message.send_date));
    std::string date = std::format("{:%Y-%m-%d}", date_timepoint);
    return ft::Renderer([=, this]{
        return ft::vbox({
            ft::text("Author: " + message.author + deliminator + "Date: " + date),
            ft::separator(),
            ft::separatorEmpty(),
            ft::text(message.subject) | ft::bold,
            ft::separatorEmpty(),
            ft::vbox({utils::split(message.content)}) | ft::yframe,
            ft::filler(),
            ft::separator(),
            ft::text(quit_message)
        });
    }) | ft::CatchEvent(utils::exit_active_screen_on_keybind());
}

void messages::content_display(
ft::Component content_component,
api* api,
size_t* redirect,
std::mutex* redirect_mutex) {
    messages_ = api->get_messages();
    struct {
        std::vector<api::content_t*> recieved;
        std::vector<api::content_t*> sent;
    } contents;

    contents.recieved.reserve(messages_.recieved.size());
    contents.sent.reserve(messages_.sent.size());
    for(const auto& message : messages_.recieved)
        contents.recieved.emplace_back((api::content_t*)&message);

    for(const auto& message : messages_.sent)
        contents.sent.emplace_back((api::content_t*)&message);
    
    const std::vector<std::string> menu_labels = { "Recieved", "Sent" };
    auto message_type_menu = ft::Menu(menu_labels, &messages_type_selected_, ft::MenuOption::HorizontalAnimated());

    auto on_action = [=]{
        auto* screen = ft::ScreenInteractive::Active();
        screen ? screen->Exit() : throw error::volumen_exception(__FUNCTION__, "", error::no_active_screen_error);
        std::lock_guard lock(*redirect_mutex);
        *redirect = main_ui::MESSAGE_VIEW; 
    };

    auto message_types_container = ft::Container::Tab({
        custom_ui::content_boxes(contents.recieved, &message_selected_, on_action),
        custom_ui::content_boxes(contents.sent, &message_selected_, on_action),
    }, &messages_type_selected_);

    auto message_components = ft::Container::Vertical({
        message_type_menu,
        message_types_container
    });
 
    // Remove loading screen
    content_component->ChildAt(0)->Detach();
    content_component->Add(message_components);
}
