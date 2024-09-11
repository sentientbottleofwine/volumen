#pragma once
#include "authorization.hpp"
#include <string>
#include <curlpp/Easy.hpp>

namespace spd = spdlog;
namespace cl = cURLpp;
using json = nlohmann::json;

class api {
    static const std::string LIBRUS_API_URL;
    static const std::string EVENT_ENDPOINT;
    static const std::string TIMETABLE_ENDPOINT;
    static const std::string TODAY_ENDPOINT;
    static const std::string MESSAGE_ENDPOINT;
    static authorization::synergia_account_t synergia_account; // Gets set once during init than stays the same
    static std::list<std::string> auth_header;
    static std::shared_ptr<std::string> fetch_id(const std::string& url_id, cl::Easy& request);

public:
    // TODO: return const
    struct event_t {
        std::string start_date;
        std::string end_date;
        std::string subject;
        std::string content;
        std::string author;
    };

    struct lesson_t {
        std::string subject;
        std::string teacher;
        std::string start;
        std::string end;
        std::string date;
        std::string substitution_note;
        bool is_substitution;
        bool is_canceled;
        bool is_empty;
    };

    struct timetable_t {
        std::shared_ptr<std::shared_ptr<std::vector<lesson_t>>[]> timetable;
        std::string prev_url;
        std::string next_url;
    };

    struct message_t {
        std::string subject;
        std::string content;
        std::string sender;
        int send_date; // You might ask why did they use a date as a string in the lesson but now they are using unix timestamps. That my friend is a question that I have yet to answer.
    };

    struct messages_t {
        std::shared_ptr<std::vector<message_t>> messages;
    };

    api(authorization::synergia_account_t& account);

    static void request_setup(cl::Easy& request, std::ostringstream& stream, const std::string& url);

    std::shared_ptr<std::vector<event_t>> get_events();

    std::shared_ptr<timetable_t>
    get_timetable(std::string week_start = "");

    std::string get_today();

    std::shared_ptr<api::messages_t>
    get_messages();
};

