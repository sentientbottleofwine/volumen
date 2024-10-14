#include "auth.hpp"
#include <cpr/cpr.h>
#include <iostream>
#include <sstream>
#include <regex>

void auth::authorize(const std::string& email, const std::string& password) {
    const std::string authcode = get_authcode(email, password);
    auto synergia_creds = fetch_portal_access_token(authcode);
    fetch_synergia_accounts(synergia_creds);
}

std::string auth::get_api_access_token(const std::string& login) const {
    assert(api_access_tokens.count(login));
    return api_access_tokens.at(login);
}

std::vector<auth::synergia_account_t> auth::get_synergia_accounts() const {
    assert(!synergia_accounts.empty());
    return synergia_accounts;
}

// After a request is sent with the bearer token for portal.librus.pl the endpoint will provide all Synergia accounts(and access tokens to said accounts) asociated with the konto librus acc
void auth::fetch_synergia_accounts(const oauth_data_t& oauth_data) {
    cpr::Response r = cpr::Get(
        cpr::Url{LIBRUS_API_ACCESS_TOKEN_URL},
        cpr::Bearer{oauth_data.access_token}
    );

    json data = json::parse(r.text);

    // TODO: If fails log where it happened
    for(const auto& account : data["accounts"].items()) {
        synergia_accounts.emplace_back(
            /*.group = */           account.value()["group"],
            /*.student_name = */    account.value()["studentName"],
            /*.login = */           account.value()["login"]
        );

        api_access_tokens.emplace(
            account.value()["login"],
            account.value()["accessToken"]
        );
    }
}

auth::oauth_data_t auth::fetch_portal_access_token(const std::string& authcode) {
    cpr::Multipart access_token_data {
        { "grant_type",      "authorization_code" },
        { "client_id",       LIBRUS_PORTAL_CLIENT_ID },
        { "redirect_uri",    LIBRUS_PORTAL_APP_URL },
        { "code",            authcode }
    };
    cpr::Response r = cpr::Post(
        cpr::Url{ LIBRUS_PORTAL_OAUTH_URL },
        access_token_data
    );

    json data = json::parse(r.text);

    return { 
        .token_type     = data["token_type"],
        .expires_in     = data["expires_in"],
        .access_token   = data["access_token"],
        .refresh_token  = data["refresh_token"]
    };
}

std::string auth::find_token(cpr::Cookies& cookies) {
    const int TOKEN_SIZE = 40; 
    cpr::Response r = cpr::Get(cpr::Url(LIBRUS_PORTAL_AUTHORIZE_URL));

    cookies = r.cookies;

    std::regex token_regex("^.*input.*_token.*value=\"", std::regex::grep);
    std::smatch token_match;

    if(!std::regex_search(r.text, token_match, token_regex))
        throw std::logic_error("Couldn't find _token in response:\n"  + r.text);

    std::string token_suffix = token_match.suffix();

    return token_suffix.substr(0,TOKEN_SIZE);
}

std::string auth::get_authcode(const std::string& email, const std::string& password) {
    cpr::Cookies cookies;
    std::string _token = find_token(cookies);

	cpr::Response r = cpr::Post(
		cpr::Url{LIBRUS_PORTAL_LOGIN_URL},
		cpr::Payload{
			{ "redirectTo", 	redirectTo	},
			{ "redirectCrc", 	redirectCrc	},
			{ "email", 			email		},
			{ "password", 		password	},
			{ "_token", 		_token		}
		},
        cpr::Redirect(cpr::PostRedirectFlags::POST_301), // To respect RFC(I don't really care but, why not)
        cookies
	);

    // Search for authcode in "location" header
    std::string authcode;
    std::regex authcode_regex(LIBRUS_PORTAL_APP_URL + "?code=", std::regex::grep);
    std::smatch authcode_match;

    if(!std::regex_search(r.header["location"], authcode_match, authcode_regex))
        throw std::runtime_error("LOGIN FAILURE: Couldn't find auth code(most likely wrong email or passoword)");
	
    const std::string authcode_suffix = authcode_match.suffix();
    authcode = authcode_suffix.substr(0, authcode_suffix.find('&'));

    return authcode;
}