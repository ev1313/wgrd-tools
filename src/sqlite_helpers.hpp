#pragma once

#include "sqlite3.h"
#include <spdlog/spdlog.h>
#include <utility>

// add this type to your input parameter list to bind a NULL value
struct SQLNULL {};

template <int BindCount = -1, int ColumnCount = -1> class SQLStatement {
private:
  sqlite3_stmt *stmt = nullptr;

  int m_index = 1;

public:
  ~SQLStatement() {
    if (!stmt) {
      return;
    }
    sqlite3_finalize(stmt);
  }
  bool init(sqlite3 *db, const char *query) {
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
      spdlog::error("Failed to prepare statement: {}", sqlite3_errmsg(db));
      return false;
    }
    return true;
  }

private:
  bool reset() {
    m_index = 1;
    int res = sqlite3_reset(stmt);
    if (res != SQLITE_OK) {
      spdlog::error("Failed to reset statement: {}",
                    sqlite3_errmsg(sqlite3_db_handle(stmt)));
      return false;
    }
    return true;
  }

  template <typename T> bool bind(T value) {
    int rc = -1;
    if constexpr (std::is_integral<T>()) {
      rc = sqlite3_bind_int64(stmt, m_index, value);
    } else if constexpr (std::is_floating_point<T>()) {
      rc = sqlite3_bind_double(stmt, m_index, value);
    } else if constexpr (std::is_same<T, std::string>()) {
      rc = sqlite3_bind_text(stmt, m_index, value.c_str(), value.size() + 1,
                             SQLITE_TRANSIENT);
    } else if constexpr (std::is_same<T, SQLNULL>()) {
      rc = sqlite3_bind_null(stmt, m_index);
    } else {
      static_assert(false, "Unsupported type!");
    }
    if (rc != SQLITE_OK) {
      spdlog::error("Failed to bind value: {}",
                    sqlite3_errmsg(sqlite3_db_handle(stmt)));
      return false;
    }
    m_index += 1;
    return true;
  }

  template <typename T, typename... Ts> bool bind(T elem, Ts &&...args) {
    if constexpr (BindCount != -1) {
      static_assert(BindCount == sizeof...(Ts));
    }
    bool ret = bind(elem);
    ret = ret && bind(std::forward<Ts>(args)...);
    if (!ret) {
      spdlog::error("Failed to bind values: {}",
                    sqlite3_errmsg(sqlite3_db_handle(stmt)));
      return false;
    }
    return true;
  }

public:
  template <typename... Ts> bool execute(Ts &&...args) {
    reset();
    if constexpr (sizeof...(args) > 0) {
      if (!bind(std::forward<Ts>(args)...)) {
        return false;
      }
    }
    auto rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
      spdlog::error("Failed to execute statement: {}",
                    sqlite3_errmsg(sqlite3_db_handle(stmt)));
      return false;
    }
    return true;
  }
  template <typename... Ts> std::optional<size_t> insert(Ts &&...args) {
    bool ret = execute(args...);
    if (!ret) {
      return std::nullopt;
    }
    return sqlite3_last_insert_rowid(sqlite3_db_handle(stmt));
  }

private:
  void get_column(int64_t &result) {
    result = sqlite3_column_int64(stmt, m_index++);
  }
  void get_column(double &result) {
    result = sqlite3_column_double(stmt, m_index++);
  }
  void get_column(std::string &str) {
    const char *text =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, m_index++));
    str = text;
  }
  template <typename T> void get_column(T &result) {
    if constexpr (std::is_integral<T>()) {
      result = sqlite3_column_int64(stmt, m_index);
    } else if constexpr (std::is_floating_point<T>()) {
      result = sqlite3_column_double(stmt, m_index);
    } else if constexpr (std::is_same<T, std::string>()) {
      const char *text =
          reinterpret_cast<const char *>(sqlite3_column_text(stmt, m_index++));
      result = text;
    } else {
      static_assert(false, "Unsupported type!");
    }
    m_index += 1;
  }

  template <typename TupleT, std::size_t... Is, typename... Ts>
  std::optional<std::vector<TupleT>> query(std::index_sequence<Is...>,
                                           Ts &&...args) {
    if constexpr (BindCount != -1) {
      static_assert(BindCount == sizeof...(Ts));
    }
    reset();
    if constexpr (sizeof...(args) > 0) {
      if (!bind(std::forward<Ts>(args)...)) {
        return std::nullopt;
      }
    }
    std::vector<TupleT> result;
    auto rc = sqlite3_step(stmt);
    while (rc == SQLITE_ROW) {
      m_index = 0;
      TupleT col;
      (get_column(std::get<Is>(col)), ...);
      rc = sqlite3_step(stmt);
      result.push_back(std::move(col));
    }
    if (rc != SQLITE_DONE) {
      spdlog::error("Failed to execute statement: {}",
                    sqlite3_errmsg(sqlite3_db_handle(stmt)));
      return std::nullopt;
    }

    return result;
  }

public:
  template <typename TupleT, std::size_t TupSize = std::tuple_size_v<TupleT>,
            typename... Ts>
  std::optional<std::vector<TupleT>> query(Ts &&...args) {
    if constexpr (ColumnCount != -1) {
      static_assert(ColumnCount == TupSize);
    }
    return query<TupleT>(std::make_index_sequence<TupSize>{}, args...);
  }
};
