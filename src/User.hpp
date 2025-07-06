#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <chrono>

class User {
public:
  enum class Gender {
    Male,
    Female,
    NonBinary,
    Other,
    Unspecified
  };

  static User create_user_safely(
    std::string username,
    std::string email,
    std::string phone_number,
    std::vector<uint8_t> profile_picture,
    std::string name,
    std::string surname,
    std::chrono::year_month_day date,
    std::string gender);

  User(const User& other) noexcept;
  User(User&& other) noexcept;
  User& operator=(const User& other) noexcept;
  User& operator=(User&& other) noexcept;

  ~User();

  const std::string& username() const;
  const std::string& email() const;
  const std::string& phone_number() const;
  const std::vector<uint8_t>& profile_picture() const;
  const std::string& name() const;
  const std::string& surname() const;
  std::chrono::year_month_day date() const;
  Gender gender() const;
  std::string gender_string() const;

  static std::string gender_to_string(Gender g);
  static Gender gender_from_string(const std::string& s);
  static std::string date_to_string(const std::chrono::year_month_day& ymd);

  static void require_non_empty(const std::string& field, const std::string& value);

  static bool is_valid_username(const std::string& username);
  static bool is_valid_email(const std::string& email);
  static bool is_valid_phone(const std::string& phone);
  static bool is_valid_name(const std::string& name);
  static bool is_valid_gender_string(const std::string& gender);
  static bool is_valid_date(const std::chrono::year_month_day& ymd);

protected:
  User(std::string username,
       std::string email,
       std::string phone_number,
       std::vector<uint8_t> profile_picture,
       std::string name,
       std::string surname,
       std::chrono::year_month_day date,
       std::string gender);

  User(std::string username,
       std::string email,
       std::string phone_number,
       std::vector<uint8_t> profile_picture,
       std::string name,
       std::string surname,
       std::chrono::year_month_day date,
       Gender gender);

private:
  std::string m_username;
  std::string m_email;
  std::string m_phone_number;
  std::string m_profile_picture_path;
  std::vector<uint8_t> m_profile_picture;
  std::string m_name;
  std::string m_surname;
  std::chrono::year_month_day m_date;
  Gender m_gender;
};

