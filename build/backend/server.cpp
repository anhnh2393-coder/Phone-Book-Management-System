#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mysql.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

#pragma comment(lib, "Ws2_32.lib")
const int PORT = 8080;

const char* DB_HOST = "127.0.0.1";
const char* DB_USER = "root";
const char* DB_PASSWORD = "123456";
const char* DB_NAME = "webdb";

std::string sqlQuote(MYSQL* connection, const std::string& value) {
    std::string escaped(value.size() * 2 + 1, '\0');
    const unsigned long length = mysql_real_escape_string(connection, escaped.data(), value.c_str(), static_cast<unsigned long>(value.size()));
    escaped.resize(length);
    return "'" + escaped + "'";
}

MYSQL* connectDatabase() {
    MYSQL* connection = mysql_init(nullptr);
    if (!connection || !mysql_real_connect(connection, DB_HOST, DB_USER, DB_PASSWORD, DB_NAME, 3306, nullptr, 0)) {
        if (connection) mysql_close(connection);
        return nullptr;
    }
    return connection;
}

bool databaseReady() {
    MYSQL* connection = connectDatabase();
    if (!connection) {
        std::cerr << "MySQL connection failed. Check that MariaDB is running and credentials are correct.\n";
        return false;
    }
    const char* query = "CREATE TABLE IF NOT EXISTS users (id INT UNSIGNED NOT NULL AUTO_INCREMENT, name VARCHAR(120) NOT NULL, email VARCHAR(255) NOT NULL, password_hash CHAR(64) NOT NULL, role ENUM('admin','user') NOT NULL DEFAULT 'user', created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP, PRIMARY KEY (id), UNIQUE KEY uq_users_email (email))";
    if (mysql_query(connection, query) != 0) {
        std::cerr << "Could not create users table: " << mysql_error(connection) << "\n";
        mysql_close(connection);
        return false;
    }
    if (mysql_query(connection, "SELECT COUNT(*) FROM information_schema.columns WHERE table_schema = DATABASE() AND table_name = 'users' AND column_name = 'role'") != 0) {
        std::cerr << "Could not inspect users role column: " << mysql_error(connection) << "\n";
        mysql_close(connection);
        return false;
    }
    MYSQL_RES* roleCheck = mysql_store_result(connection);
    MYSQL_ROW roleRow = roleCheck ? mysql_fetch_row(roleCheck) : nullptr;
    const bool hasRoleColumn = roleRow && roleRow[0] && std::string(roleRow[0]) != "0";
    if (roleCheck) mysql_free_result(roleCheck);
    if (!hasRoleColumn && mysql_query(connection, "ALTER TABLE users ADD COLUMN role ENUM('admin','user') NOT NULL DEFAULT 'user'") != 0) {
        std::cerr << "Could not add users role column: " << mysql_error(connection) << "\n";
        mysql_close(connection);
        return false;
    }
    const char* contactsTable = "CREATE TABLE IF NOT EXISTS contacts (id INT UNSIGNED NOT NULL AUTO_INCREMENT, user_id INT UNSIGNED NOT NULL, name VARCHAR(120) NOT NULL, phone VARCHAR(40) NOT NULL, email VARCHAR(255) NOT NULL, contact_group VARCHAR(40) NOT NULL DEFAULT 'Friends', favorite BOOLEAN NOT NULL DEFAULT FALSE, created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP, PRIMARY KEY (id), KEY idx_contacts_user_id (user_id), CONSTRAINT fk_contacts_user FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE)";
    if (mysql_query(connection, contactsTable) != 0) {
        std::cerr << "Could not create contacts table: " << mysql_error(connection) << "\n";
        mysql_close(connection);
        return false;
    }
    const char* seedAdmin = "INSERT INTO users (name, email, password_hash, role) VALUES ('admin', 'admin@university.edu', SHA2('admin123', 256), 'admin') ON DUPLICATE KEY UPDATE name = 'admin', password_hash = SHA2('admin123', 256), role = 'admin'";
    const bool ready = mysql_query(connection, seedAdmin) == 0;
    if (!ready) std::cerr << "Could not seed admin account: " << mysql_error(connection) << "\n";
    mysql_close(connection);
    return ready;
}

struct AuthenticatedUser {
    std::string name;
    std::string email;
    std::string role;
    enum class LoginStatus { Success, AccountNotFound, WrongPassword } status = LoginStatus::AccountNotFound;
};

AuthenticatedUser authenticateUser(const std::string& username, const std::string& password) {
    MYSQL* connection = connectDatabase();
    if (!connection) return {};
    const std::string query = "SELECT id, name, email, role FROM users WHERE email = " + sqlQuote(connection, username) + " OR name = " + sqlQuote(connection, username) + " LIMIT 1";
    AuthenticatedUser user;
    if (mysql_query(connection, query.c_str()) == 0) {
        MYSQL_RES* result = mysql_store_result(connection);
        std::string userId;
        if (result) {
            MYSQL_ROW row = mysql_fetch_row(result);
            if (row) {
                if (row[0]) userId = row[0];
                if (row[1]) user.name = row[1];
                if (row[2]) user.email = row[2];
                if (row[3]) user.role = row[3];
            }
            mysql_free_result(result);
        }
        if (!userId.empty()) {
            const std::string passwordQuery = "SELECT COUNT(*) FROM users WHERE id = " + userId + " AND password_hash = SHA2(" + sqlQuote(connection, password) + ", 256)";
            if (mysql_query(connection, passwordQuery.c_str()) == 0) {
                MYSQL_RES* passwordResult = mysql_store_result(connection);
                MYSQL_ROW passwordRow = passwordResult ? mysql_fetch_row(passwordResult) : nullptr;
                if (passwordRow && passwordRow[0] && std::string(passwordRow[0]) == "1") user.status = AuthenticatedUser::LoginStatus::Success;
                else user.status = AuthenticatedUser::LoginStatus::WrongPassword;
                if (passwordResult) mysql_free_result(passwordResult);
            } else {
                user.status = AuthenticatedUser::LoginStatus::WrongPassword;
            }
        }
    }
    mysql_close(connection);
    return user;
}

bool registerUser(const std::string& name, const std::string& email, const std::string& password) {
    MYSQL* connection = connectDatabase();
    if (!connection) return false;
    const std::string query = "INSERT INTO users (name, email, password_hash, role) VALUES (" + sqlQuote(connection, name) + ", " + sqlQuote(connection, email) + ", SHA2(" + sqlQuote(connection, password) + ", 256), 'user')";
    const bool registered = mysql_query(connection, query.c_str()) == 0;
    mysql_close(connection);
    return registered;
}

std::string jsonEscape(const std::string& value);

std::string getUsersJson() {
    MYSQL* connection = connectDatabase();
    if (!connection) return {};
    if (mysql_query(connection, "SELECT users.id, users.name, users.email, users.role, users.created_at, COUNT(contacts.id) FROM users LEFT JOIN contacts ON contacts.user_id = users.id GROUP BY users.id, users.name, users.email, users.role, users.created_at ORDER BY users.role DESC, users.id ASC") != 0) {
        mysql_close(connection);
        return {};
    }
    MYSQL_RES* result = mysql_store_result(connection);
    if (!result) {
        mysql_close(connection);
        return {};
    }
    std::string response = "[";
    MYSQL_ROW row;
    bool first = true;
    while ((row = mysql_fetch_row(result)) != nullptr) {
        if (!first) response += ',';
        first = false;
        response += "{\"id\":" + std::string(row[0]) + ",\"name\":\"" + jsonEscape(row[1]) + "\",\"email\":\"" + jsonEscape(row[2]) + "\",\"role\":\"" + jsonEscape(row[3]) + "\",\"createdAt\":\"" + jsonEscape(row[4]) + "\",\"contactCount\":" + std::string(row[5]) + "}";
    }
    response += ']';
    mysql_free_result(result);
    mysql_close(connection);
    return response;
}

bool deleteUser(unsigned int id) {
    MYSQL* connection = connectDatabase();
    if (!connection) return false;
    const std::string query = "DELETE FROM users WHERE id = " + std::to_string(id) + " AND role = 'user'";
    const bool deleted = mysql_query(connection, query.c_str()) == 0 && mysql_affected_rows(connection) > 0;
    mysql_close(connection);
    return deleted;
}

std::string jsonEscape(const std::string& value) {
    std::string escaped;
    for (const char character : value) {
        if (character == '\\') escaped += "\\\\";
        else if (character == '"') escaped += "\\\"";
        else if (character == '\n') escaped += "\\n";
        else escaped += character;
    }
    return escaped;
}

std::string getContactsJson(const std::string& account) {
    MYSQL* connection = connectDatabase();
    if (!connection) return {};
    const std::string query = "SELECT contacts.id, contacts.name, contacts.phone, contacts.email, contacts.contact_group, contacts.favorite FROM contacts INNER JOIN users ON users.id = contacts.user_id WHERE users.email = " + sqlQuote(connection, account) + " ORDER BY contacts.id";
    if (mysql_query(connection, query.c_str()) != 0) {
        mysql_close(connection);
        return {};
    }
    MYSQL_RES* result = mysql_store_result(connection);
    if (!result) {
        mysql_close(connection);
        return {};
    }
    std::string response = "[";
    MYSQL_ROW row;
    bool first = true;
    while ((row = mysql_fetch_row(result)) != nullptr) {
        if (!first) response += ',';
        first = false;
        response += "{\"id\":" + std::string(row[0]) + ",\"name\":\"" + jsonEscape(row[1]) + "\",\"phone\":\"" + jsonEscape(row[2]) + "\",\"email\":\"" + jsonEscape(row[3]) + "\",\"group\":\"" + jsonEscape(row[4]) + "\",\"favorite\":" + (std::string(row[5]) == "1" ? "true" : "false") + "}";
    }
    response += ']';
    mysql_free_result(result);
    mysql_close(connection);
    return response;
}

bool createContact(const std::string& account, const std::string& name, const std::string& phone, const std::string& email, const std::string& group) {
    MYSQL* connection = connectDatabase();
    if (!connection) return false;
    const std::string query = "INSERT INTO contacts (user_id, name, phone, email, contact_group) SELECT id, " + sqlQuote(connection, name) + ", " + sqlQuote(connection, phone) + ", " + sqlQuote(connection, email) + ", " + sqlQuote(connection, group) + " FROM users WHERE email = " + sqlQuote(connection, account) + " LIMIT 1";
    const bool created = mysql_query(connection, query.c_str()) == 0;
    if (!created) std::cerr << "Could not save contact: " << mysql_error(connection) << "\n";
    mysql_close(connection);
    return created;
}

bool updateContact(const std::string& account, unsigned int id, const std::string& name, const std::string& phone, const std::string& email, const std::string& group) {
    MYSQL* connection = connectDatabase();
    if (!connection) return false;
    const std::string query = "UPDATE contacts INNER JOIN users ON users.id = contacts.user_id SET contacts.name = " + sqlQuote(connection, name) + ", contacts.phone = " + sqlQuote(connection, phone) + ", contacts.email = " + sqlQuote(connection, email) + ", contacts.contact_group = " + sqlQuote(connection, group) + " WHERE contacts.id = " + std::to_string(id) + " AND users.email = " + sqlQuote(connection, account);
    const bool updated = mysql_query(connection, query.c_str()) == 0;
    if (!updated) std::cerr << "Could not update contact: " << mysql_error(connection) << "\n";
    mysql_close(connection);
    return updated;
}

bool deleteContact(const std::string& account, unsigned int id) {
    MYSQL* connection = connectDatabase();
    if (!connection) return false;
    const std::string query = "DELETE contacts FROM contacts INNER JOIN users ON users.id = contacts.user_id WHERE contacts.id = " + std::to_string(id) + " AND users.email = " + sqlQuote(connection, account);
    const bool deleted = mysql_query(connection, query.c_str()) == 0 && mysql_affected_rows(connection) > 0;
    if (!deleted) std::cerr << "Could not delete contact: " << mysql_error(connection) << "\n";
    mysql_close(connection);
    return deleted;
}

std::string readFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return {};
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

std::string urlDecode(const std::string& value) {
    std::string result;
    for (size_t i = 0; i < value.size(); ++i) {
        if (value[i] == '+') {
            result += ' ';
        } else if (value[i] == '%' && i + 2 < value.size()) {
            result += static_cast<char>(std::stoi(value.substr(i + 1, 2), nullptr, 16));
            i += 2;
        } else {
            result += value[i];
        }
    }
    return result;
}

std::unordered_map<std::string, std::string> parseForm(const std::string& body) {
    std::unordered_map<std::string, std::string> form;
    std::stringstream stream(body);
    std::string item;
    while (std::getline(stream, item, '&')) {
        const size_t separator = item.find('=');
        if (separator != std::string::npos) {
            form[urlDecode(item.substr(0, separator))] = urlDecode(item.substr(separator + 1));
        }
    }
    return form;
}

void sendResponse(SOCKET client, const std::string& status, const std::string& contentType, const std::string& body) {
    std::ostringstream response;
    response << "HTTP/1.1 " << status << "\r\n"
             << "Content-Type: " << contentType << "\r\n"
             << "Content-Length: " << body.size() << "\r\n"
             << "Connection: close\r\n\r\n"
             << body;
    const std::string message = response.str();
    send(client, message.c_str(), static_cast<int>(message.size()), 0);
}

void redirect(SOCKET client, const std::string& location, const std::string& role = {}, const std::string& account = {}) {
    const std::string cookie = role.empty() ? "" : "Set-Cookie: role=" + role + "; Path=/; HttpOnly\r\nSet-Cookie: account=" + account + "; Path=/; HttpOnly\r\n";
    const std::string response = "HTTP/1.1 302 Found\r\nLocation: " + location + "\r\n" + cookie + "Content-Length: 0\r\nConnection: close\r\n\r\n";
    send(client, response.c_str(), static_cast<int>(response.size()), 0);
}

bool hasAdminCookie(const std::string& request) {
    const size_t cookieStart = request.find("Cookie:");
    if (cookieStart == std::string::npos) return false;
    const size_t lineEnd = request.find("\r\n", cookieStart);
    const std::string cookieLine = request.substr(cookieStart, lineEnd == std::string::npos ? std::string::npos : lineEnd - cookieStart);
    return cookieLine.find("role=admin") != std::string::npos;
}

std::string getCookie(const std::string& request, const std::string& name) {
    const size_t cookieStart = request.find("Cookie:");
    if (cookieStart == std::string::npos) return {};
    const size_t lineEnd = request.find("\r\n", cookieStart);
    const std::string cookieLine = request.substr(cookieStart, lineEnd == std::string::npos ? std::string::npos : lineEnd - cookieStart);
    const std::string prefix = name + "=";
    const size_t valueStart = cookieLine.find(prefix);
    if (valueStart == std::string::npos) return {};
    const size_t valueEnd = cookieLine.find(';', valueStart + prefix.size());
    return cookieLine.substr(valueStart + prefix.size(), valueEnd == std::string::npos ? std::string::npos : valueEnd - valueStart - prefix.size());
}

std::string getCurrentUserJson(const std::string& request) {
    const std::string account = getCookie(request, "account");
    if (account.empty()) return {};
    MYSQL* connection = connectDatabase();
    if (!connection) return {};
    const std::string query = "SELECT name, email, role FROM users WHERE email = " + sqlQuote(connection, account) + " LIMIT 1";
    if (mysql_query(connection, query.c_str()) != 0) {
        mysql_close(connection);
        return {};
    }
    MYSQL_RES* result = mysql_store_result(connection);
    MYSQL_ROW row = result ? mysql_fetch_row(result) : nullptr;
    std::string response;
    if (row) response = "{\"name\":\"" + jsonEscape(row[0]) + "\",\"email\":\"" + jsonEscape(row[1]) + "\",\"role\":\"" + jsonEscape(row[2]) + "\"}";
    if (result) mysql_free_result(result);
    mysql_close(connection);
    return response;
}

void handleRequest(SOCKET client) {
    char buffer[16384] = {};
    const int received = recv(client, buffer, sizeof(buffer) - 1, 0);
    if (received <= 0) return;

    const std::string request(buffer, received);
    const size_t lineEnd = request.find("\r\n");
    const std::string requestLine = request.substr(0, lineEnd);
    std::stringstream line(requestLine);
    std::string method;
    std::string path;
    line >> method >> path;

    const size_t bodyStart = request.find("\r\n\r\n");
    const std::string body = bodyStart == std::string::npos ? "" : request.substr(bodyStart + 4);

    if (method == "GET" && path == "/api/contacts") {
        const std::string account = getCookie(request, "account");
        if (account.empty()) {
            sendResponse(client, "401 Unauthorized", "text/plain; charset=utf-8", "Login required");
            return;
        }
        const std::string contacts = getContactsJson(account);
        if (contacts.empty()) sendResponse(client, "500 Internal Server Error", "text/plain; charset=utf-8", "Could not load contacts");
        else sendResponse(client, "200 OK", "application/json; charset=utf-8", contacts);
        return;
    }

    if (method == "GET" && path == "/api/users") {
        if (!hasAdminCookie(request)) {
            sendResponse(client, "403 Forbidden", "text/plain; charset=utf-8", "Admin access required");
            return;
        }
        const std::string users = getUsersJson();
        if (users.empty()) sendResponse(client, "500 Internal Server Error", "text/plain; charset=utf-8", "Could not load users");
        else sendResponse(client, "200 OK", "application/json; charset=utf-8", users);
        return;
    }

    if (method == "GET" && path == "/api/me") {
        const std::string user = getCurrentUserJson(request);
        if (user.empty()) sendResponse(client, "401 Unauthorized", "text/plain; charset=utf-8", "Login required");
        else sendResponse(client, "200 OK", "application/json; charset=utf-8", user);
        return;
    }

    if (method == "DELETE" && path == "/api/users") {
        if (!hasAdminCookie(request)) {
            sendResponse(client, "403 Forbidden", "text/plain; charset=utf-8", "Admin access required");
            return;
        }
        const auto form = parseForm(body);
        if (form.count("id") && deleteUser(static_cast<unsigned int>(std::stoul(form.at("id"))))) {
            sendResponse(client, "200 OK", "application/json; charset=utf-8", "{\"deleted\":true}");
        } else {
            sendResponse(client, "400 Bad Request", "application/json; charset=utf-8", "{\"deleted\":false}");
        }
        return;
    }

    if (method == "POST" && path == "/api/contacts") {
        const auto form = parseForm(body);
        const std::string account = getCookie(request, "account");
        if (account.empty()) {
            sendResponse(client, "401 Unauthorized", "text/plain; charset=utf-8", "Login required");
        } else if (form.count("name") && form.count("phone") && form.count("email") && form.count("group") && createContact(account, form.at("name"), form.at("phone"), form.at("email"), form.at("group"))) {
            sendResponse(client, "201 Created", "application/json; charset=utf-8", "{\"saved\":true}");
        } else {
            sendResponse(client, "400 Bad Request", "application/json; charset=utf-8", "{\"saved\":false}");
        }
        return;
    }

    if (method == "PUT" && path == "/api/contacts") {
        const auto form = parseForm(body);
        const std::string account = getCookie(request, "account");
        if (account.empty()) {
            sendResponse(client, "401 Unauthorized", "application/json; charset=utf-8", "{\"saved\":false}");
        } else if (form.count("id") && form.count("name") && form.count("phone") && form.count("email") && form.count("group") && updateContact(account, static_cast<unsigned int>(std::stoul(form.at("id"))), form.at("name"), form.at("phone"), form.at("email"), form.at("group"))) {
            sendResponse(client, "200 OK", "application/json; charset=utf-8", "{\"saved\":true}");
        } else {
            sendResponse(client, "400 Bad Request", "application/json; charset=utf-8", "{\"saved\":false}");
        }
        return;
    }

    if (method == "DELETE" && path == "/api/contacts") {
        const auto form = parseForm(body);
        const std::string account = getCookie(request, "account");
        if (account.empty()) {
            sendResponse(client, "401 Unauthorized", "application/json; charset=utf-8", "{\"deleted\":false}");
        } else if (form.count("id") && deleteContact(account, static_cast<unsigned int>(std::stoul(form.at("id"))))) {
            sendResponse(client, "200 OK", "application/json; charset=utf-8", "{\"deleted\":true}");
        } else {
            sendResponse(client, "400 Bad Request", "application/json; charset=utf-8", "{\"deleted\":false}");
        }
        return;
    }

    if (method == "POST" && path == "/login") {
        const auto form = parseForm(body);
        if (form.count("username") && form.count("password") && !form.at("username").empty() && !form.at("password").empty()) {
            const AuthenticatedUser user = authenticateUser(form.at("username"), form.at("password"));
            if (user.status == AuthenticatedUser::LoginStatus::Success && user.role == "admin") {
                redirect(client, "/admin", "admin", user.email);
            } else if (user.status == AuthenticatedUser::LoginStatus::Success && user.role == "user") {
                redirect(client, "/dashboard", "user", user.email);
            } else if (user.status == AuthenticatedUser::LoginStatus::AccountNotFound) {
                redirect(client, "/login?error=account");
            } else {
                redirect(client, "/login?error=password");
            }
        } else {
            redirect(client, "/login?error=empty");
        }
        return;
    }

    if (method == "POST" && path == "/register") {
        const auto form = parseForm(body);
        if (form.count("name") && form.count("email") && form.count("password") && registerUser(form.at("name"), form.at("email"), form.at("password"))) {
            redirect(client, "/login?registered=1");
        } else {
            redirect(client, "/register?error=duplicate");
        }
        return;
    }

    if (method != "GET") {
        sendResponse(client, "405 Method Not Allowed", "text/plain; charset=utf-8", "Method Not Allowed");
        return;
    }

    const size_t queryStart = path.find('?');
    if (queryStart != std::string::npos) path = path.substr(0, queryStart);

    std::string file;
    if (path == "/" || path == "/login" || path == "/login/") file = "frontend/index.html";
    else if (path == "/register" || path == "/register/") file = "frontend/register.html";
    else if (path == "/admin" || path == "/admin/") {
        if (!hasAdminCookie(request)) {
            redirect(client, "/login?error=admin");
            return;
        }
        file = "frontend/admin.html";
    }
    else if (path == "/dashboard" || path == "/dashboard/") file = "frontend/dashboard.html";
    else if (path == "/style.css") file = "frontend/style.css";
    else {
        sendResponse(client, "404 Not Found", "text/plain; charset=utf-8", "Page not found");
        return;
    }

    const std::string content = readFile(file);
    if (content.empty()) {
        sendResponse(client, "500 Internal Server Error", "text/plain; charset=utf-8", "Could not read page");
        return;
    }
    const std::string contentType = path == "/style.css" ? "text/css; charset=utf-8" : "text/html; charset=utf-8";
    sendResponse(client, "200 OK", contentType, content);
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }

    if (!databaseReady()) {
        std::cerr << "Could not connect to MySQL database webdb\n";
        WSACleanup();
        return 1;
    }

    SOCKET server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server == INVALID_SOCKET) {
        std::cerr << "Could not create socket\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(PORT);

    if (bind(server, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == SOCKET_ERROR || listen(server, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Could not start server on port " << PORT << "\n";
        closesocket(server);
        WSACleanup();
        return 1;
    }

    std::cout << "Phone Book server running at http://localhost:" << PORT << "/login\n";
    while (true) {
        SOCKET client = accept(server, nullptr, nullptr);
        if (client == INVALID_SOCKET) break;
        handleRequest(client);
        closesocket(client);
    }

    closesocket(server);
    WSACleanup();
    return 0;
}
