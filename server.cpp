#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mysql.h>
#include <algorithm>
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
    if (!connection) return false;
    const char* query = "CREATE TABLE IF NOT EXISTS users (id INT UNSIGNED NOT NULL AUTO_INCREMENT, name VARCHAR(120) NOT NULL, email VARCHAR(255) NOT NULL, password_hash CHAR(64) NOT NULL, created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP, PRIMARY KEY (id), UNIQUE KEY uq_users_email (email))";
    if (mysql_query(connection, query) != 0) {
        mysql_close(connection);
        return false;
    }
    const char* contactsTable = "CREATE TABLE IF NOT EXISTS contacts (id INT UNSIGNED NOT NULL AUTO_INCREMENT, user_id INT UNSIGNED NOT NULL, name VARCHAR(120) NOT NULL, phone VARCHAR(40) NOT NULL, email VARCHAR(255) NOT NULL, contact_group VARCHAR(40) NOT NULL DEFAULT 'Friends', favorite BOOLEAN NOT NULL DEFAULT FALSE, created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP, PRIMARY KEY (id), KEY idx_contacts_user_id (user_id), CONSTRAINT fk_contacts_user FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE)";
    if (mysql_query(connection, contactsTable) != 0) {
        mysql_close(connection);
        return false;
    }
    const char* seedAdmin = "INSERT INTO users (name, email, password_hash) VALUES ('admin', 'admin@university.edu', SHA2('admin123', 256)) ON DUPLICATE KEY UPDATE name = 'admin', password_hash = SHA2('admin123', 256)";
    const bool ready = mysql_query(connection, seedAdmin) == 0;
    mysql_close(connection);
    return ready;
}

bool authenticateUser(const std::string& username, const std::string& password) {
    MYSQL* connection = connectDatabase();
    if (!connection) return false;
    const std::string query = "SELECT id FROM users WHERE (email = " + sqlQuote(connection, username) + " OR name = " + sqlQuote(connection, username) + ") AND password_hash = SHA2(" + sqlQuote(connection, password) + ", 256) LIMIT 1";
    bool authenticated = false;
    if (mysql_query(connection, query.c_str()) == 0) {
        MYSQL_RES* result = mysql_store_result(connection);
        authenticated = result && mysql_num_rows(result) > 0;
        if (result) mysql_free_result(result);
    }
    mysql_close(connection);
    return authenticated;
}

bool registerUser(const std::string& name, const std::string& email, const std::string& password) {
    MYSQL* connection = connectDatabase();
    if (!connection) return false;
    const std::string query = "INSERT INTO users (name, email, password_hash) VALUES (" + sqlQuote(connection, name) + ", " + sqlQuote(connection, email) + ", SHA2(" + sqlQuote(connection, password) + ", 256))";
    const bool registered = mysql_query(connection, query.c_str()) == 0;
    mysql_close(connection);
    return registered;
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

std::string getContactsJson() {
    MYSQL* connection = connectDatabase();
    if (!connection) return {};
    const char* query = "SELECT contacts.id, contacts.name, contacts.phone, contacts.email, contacts.contact_group, contacts.favorite FROM contacts INNER JOIN users ON users.id = contacts.user_id WHERE users.email = 'admin@university.edu' ORDER BY contacts.id";
    if (mysql_query(connection, query) != 0) {
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

bool createContact(const std::string& name, const std::string& phone, const std::string& email, const std::string& group) {
    MYSQL* connection = connectDatabase();
    if (!connection) return false;
    const std::string query = "INSERT INTO contacts (user_id, name, phone, email, contact_group) SELECT id, " + sqlQuote(connection, name) + ", " + sqlQuote(connection, phone) + ", " + sqlQuote(connection, email) + ", " + sqlQuote(connection, group) + " FROM users WHERE email = 'admin@university.edu' LIMIT 1";
    const bool created = mysql_query(connection, query.c_str()) == 0;
    if (!created) std::cerr << "Could not save contact: " << mysql_error(connection) << "\n";
    mysql_close(connection);
    return created;
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

void redirect(SOCKET client, const std::string& location) {
    const std::string response = "HTTP/1.1 302 Found\r\nLocation: " + location + "\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
    send(client, response.c_str(), static_cast<int>(response.size()), 0);
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
        const std::string contacts = getContactsJson();
        if (contacts.empty()) sendResponse(client, "500 Internal Server Error", "text/plain; charset=utf-8", "Could not load contacts");
        else sendResponse(client, "200 OK", "application/json; charset=utf-8", contacts);
        return;
    }

    if (method == "POST" && path == "/api/contacts") {
        const auto form = parseForm(body);
        if (form.count("name") && form.count("phone") && form.count("email") && form.count("group") && createContact(form.at("name"), form.at("phone"), form.at("email"), form.at("group"))) {
            sendResponse(client, "201 Created", "application/json; charset=utf-8", "{\"saved\":true}");
        } else {
            sendResponse(client, "400 Bad Request", "application/json; charset=utf-8", "{\"saved\":false}");
        }
        return;
    }

    if (method == "POST" && path == "/login") {
        const auto form = parseForm(body);
        if (form.count("username") && form.count("password") && authenticateUser(form.at("username"), form.at("password"))) {
            redirect(client, "/dashboard");
        } else {
            redirect(client, "/login?error=invalid");
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
    if (path == "/" || path == "/login" || path == "/login/") file = "index.html";
    else if (path == "/register" || path == "/register/") file = "register.html";
    else if (path == "/dashboard" || path == "/dashboard/") file = "dashboard.html";
    else if (path == "/style.css") file = "style.css";
    else {
        sendResponse(client, "404 Not Found", "text/plain; charset=utf-8", "Page not found");
        return;
    }

    const std::string content = readFile(file);
    if (content.empty()) {
        sendResponse(client, "500 Internal Server Error", "text/plain; charset=utf-8", "Could not read page");
        return;
    }
    const std::string contentType = file == "style.css" ? "text/css; charset=utf-8" : "text/html; charset=utf-8";
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
