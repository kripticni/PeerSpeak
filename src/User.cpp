#pragma once

#include <../include/User.hpp>

#include <SQLiteCpp/SQLiteCpp.h>
#include <string>
#include <vector>
#include <cstdint>
#include <chrono>
#include <regex>
#include <stdexcept>
#include <format>


User User::create_user_safely(
    std::string username,
    std::string email,
    std::string phone_number,
    std::vector<uint8_t> profile_picture,
    std::string name,
    std::string surname,
    std::chrono::year_month_day date,
    std::string gender)
  {
    if (!is_valid_username(username)) throw std::invalid_argument("Invalid username");
    if (!is_valid_email(email)) throw std::invalid_argument("Invalid email format");
    if (!is_valid_phone(phone_number)) throw std::invalid_argument("Invalid phone number");
    if (!is_valid_name(name)) throw std::invalid_argument("Invalid name");
    if (!is_valid_name(surname)) throw std::invalid_argument("Invalid surname");
    if (!is_valid_date(date)) throw std::invalid_argument("Invalid date");
    if (!is_valid_gender_string(gender)) throw std::invalid_argument("Invalid gender (expected: male, female, non-binary, other)");
    return User(std::move(username), std::move(email), std::move(phone_number), 
                std::move(profile_picture), std::move(name), std::move(surname),
                date, std::move(gender));
  }

User::User(const User& other) noexcept
    : m_username(other.m_username),
    m_email(other.m_email),
    m_phone_number(other.m_phone_number),
    m_profile_picture_path(other.m_profile_picture_path),
    m_profile_picture(other.m_profile_picture),
    m_name(other.m_name),
    m_surname(other.m_surname),
    m_date(other.m_date),
    m_gender(other.m_gender)
  {}

User::User(User&& other) noexcept
    : m_username(std::move(other.m_username)),
    m_email(std::move(other.m_email)),
    m_phone_number(std::move(other.m_phone_number)),
    m_profile_picture_path(std::move(other.m_profile_picture_path)),
    m_profile_picture(std::move(other.m_profile_picture)),
    m_name(std::move(other.m_name)),
    m_surname(std::move(other.m_surname)),
    m_date(other.m_date),
    m_gender(std::move(other.m_gender))
  {}

User::~User() = default;

User& User::operator=(const User& other) noexcept {
    if (this != &other) {
      m_username = other.m_username;
      m_email = other.m_email;
      m_phone_number = other.m_phone_number;
      m_profile_picture_path = other.m_profile_picture_path;
      m_profile_picture = other.m_profile_picture;
      m_name = other.m_name;
      m_surname = other.m_surname;
      m_date = other.m_date;
      m_gender = other.m_gender;
    }
    return *this;
  }

  User& User::operator=(User&& other) noexcept {
    if (this != &other) {
      m_username = std::move(other.m_username);
      m_email = std::move(other.m_email);
      m_phone_number = std::move(other.m_phone_number);
      m_profile_picture_path = std::move(other.m_profile_picture_path);
      m_profile_picture = std::move(other.m_profile_picture);
      m_name = std::move(other.m_name);
      m_surname = std::move(other.m_surname);
      m_date = other.m_date;
      m_gender = std::move(other.m_gender);
    }
    return *this;
  }

  const std::string& User::username() const { return m_username; }
  const std::string& User::email() const { return m_email; }
  const std::string& User::phone_number() const { return m_phone_number; }
  const std::vector<uint8_t>& User::profile_picture() const { return m_profile_picture; }
  const std::string& User::name() const { return m_name; }
  const std::string& User::surname() const { return m_surname; }
  const std::chrono::year_month_day User::date() const { return m_date; }
  const User::Gender User::gender() const { return m_gender; }
  const std::string User::gender_string() const { return gender_to_string(m_gender); }

  static inline const std::string gender_to_string(User::Gender g) {
    switch (g) {
      case User::Gender::Male: return "Male";
      case User::Gender::Female: return "Female";
      case User::Gender::NonBinary: return "NonBinary";
      case User::Gender::Other: return "Other";
      case User::Gender::Unspecified: return "Unspecified";
      default: return "Unknown";
    }
  }

  inline User::Gender User::gender_from_string(const std::string& s) {
    if (s == "Male") return User::Gender::Male;
    if (s == "Female") return User::Gender::Female;
    if (s == "NonBinary") return User::Gender::NonBinary;
    if (s == "Other") return User::Gender::Other;
    return User::Gender::Unspecified;
  }

  inline std::string User::date_to_string(const std::chrono::year_month_day& ymd){
    std::chrono::sys_days sd = std::chrono::sys_days{ymd};
    return std::format("{:%F}", sd);
  }

  inline void User::require_non_empty(const std::string&& field, const std::string& value) {
    if (value.empty()) throw std::invalid_argument(field + " must not be empty.");
  }

  inline void User::require_non_empty(const std::string& field, const std::string& value) {
    if (value.empty()) throw std::invalid_argument(field + " must not be empty.");
  }

  inline bool User::is_valid_username(const std::string& username) {
    User::require_non_empty("Username", username);
    static const std::regex pattern(R"(^[a-z0-9_.-]{3,32}$)");
    return std::regex_match(username, pattern);
  }

  inline bool User::is_valid_email(const std::string& email) {
    User::require_non_empty("Email", email);
    static const std::regex pattern(R"(^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+(\.[A-Za-z]{2,})?$)");
    return std::regex_match(email, pattern);
  }

  inline bool User::is_valid_phone(const std::string& phone) {
    User::require_non_empty("Phone number", phone);
    static const std::regex pattern(R"(^\+?[0-9\- ]{7,20}$)");
    return std::regex_match(phone, pattern);
  }

  inline bool User::is_valid_name(const std::string& name) {
    User::require_non_empty("Name", name);
    static const std::regex pattern(R"(^[A-Za-zÀ-ž'\-\s]{2,50}$)");
    return std::regex_match(name, pattern);
  }

  inline bool User::is_valid_gender_string(const std::string& gender) {
    User::require_non_empty("Gender", gender);
    return (gender == "Male" || gender == "Female" || gender == "Other" || gender == "NonBinary" || gender == "Other");
  }

  inline bool User::is_valid_date(const std::chrono::year_month_day& ymd) {
    return ymd.ok();
  }

  User::User(std::string username,
       std::string email,
       std::string phone_number,
       std::vector<uint8_t> profile_picture,
       std::string name,
       std::string surname,
       std::chrono::year_month_day date,
       std::string gender)
    : m_username(std::move(username)),
    m_email(std::move(email)),
    m_phone_number(std::move(phone_number)),
    m_profile_picture(std::move(profile_picture)),
    m_name(std::move(name)),
    m_surname(std::move(surname)),
    m_date(date),
    m_gender(gender_from_string(gender))
  {}

User::User(std::string username,
       std::string email,
       std::string phone_number,
       std::vector<uint8_t> profile_picture,
       std::string name,
       std::string surname,
       std::chrono::year_month_day date,
       User::Gender gender)
    : m_username(std::move(username)),
    m_email(std::move(email)),
    m_phone_number(std::move(phone_number)),
    m_profile_picture(std::move(profile_picture)),
    m_name(std::move(name)),
    m_surname(std::move(surname)),
    m_date(date),
    m_gender(gender)
  {}
