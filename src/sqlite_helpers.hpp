#pragma once

#include "sqlite3.h"
#include <spdlog/spdlog.h>
#include <type_traits>
#include <utility>

template <class T> inline constexpr bool is_tuple_like_v = false;

template <class... Elems>
inline constexpr bool is_tuple_like_v<std::tuple<Elems...>> = true;

template <class T1, class T2>
inline constexpr bool is_tuple_like_v<std::pair<T1, T2>> = true;

template <class T, size_t N>
inline constexpr bool is_tuple_like_v<std::array<T, N>> = true;

template <class It, class Sent, std::ranges::subrange_kind Kind>
inline constexpr bool is_tuple_like_v<std::ranges::subrange<It, Sent, Kind>> =
    true;

template <class T>
concept tuple_like = is_tuple_like_v<std::remove_cvref_t<T>>;

template <class T>
concept not_tuple_like = !tuple_like<T>;

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
    assert(sqlite3_threadsafe() == 1);
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
      spdlog::error("Failed to prepare statement: {}", sqlite3_errmsg(db));
      spdlog::error("query: '{}'", query);
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
    if constexpr (BindCount != -1) {
      static_assert(BindCount == sizeof...(Ts));
    }
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

  template <tuple_like TupleT, std::size_t... Is, typename... Ts>
  std::optional<std::vector<TupleT>> query_tup(std::index_sequence<Is...>,
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
  template <tuple_like TupleT, std::size_t TupSize = std::tuple_size_v<TupleT>,
            typename... Ts>
  std::optional<std::vector<TupleT>> query_tup(Ts &&...args) {
    if constexpr (ColumnCount != -1) {
      static_assert(ColumnCount == TupSize);
    }
    return query_tup<TupleT>(std::make_index_sequence<TupSize>{}, args...);
  }

public:
  template <typename T, typename... Ts>
  std::optional<std::vector<T>> query(Ts &&...args) {
    if constexpr (BindCount != -1) {
      static_assert(BindCount == sizeof...(Ts));
    }
    if constexpr (is_tuple_like_v<T>) {
      return query_tup<T>(args...);
    } else {
      if constexpr (ColumnCount != -1) {
        static_assert(ColumnCount == 1);
      }
      reset();
      if constexpr (sizeof...(args) > 0) {
        if (!bind(std::forward<Ts>(args)...)) {
          return std::nullopt;
        }
      }
      std::vector<T> result;
      auto rc = sqlite3_step(stmt);
      while (rc == SQLITE_ROW) {
        m_index = 0;
        T col;
        get_column(col);
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
  }

private:
  template <tuple_like TupleT, std::size_t... Is, typename... Ts>
  std::optional<TupleT> query_single_tup(std::index_sequence<Is...>,
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
    auto rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
      spdlog::error("Failed to execute statement: {}",
                    sqlite3_errmsg(sqlite3_db_handle(stmt)));
      return std::nullopt;
    }
    m_index = 0;
    TupleT col;
    (get_column(std::get<Is>(col)), ...);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
      spdlog::error("Failed to execute statement: {}",
                    sqlite3_errmsg(sqlite3_db_handle(stmt)));
      return std::nullopt;
    }

    return std::move(col);
  }
  template <tuple_like TupleT, std::size_t TupSize = std::tuple_size_v<TupleT>,
            typename... Ts>
  std::optional<TupleT> query_single_tup(Ts &&...args) {
    if constexpr (ColumnCount != -1) {
      static_assert(ColumnCount == TupSize);
    }
    return query_single_tup<TupleT>(std::make_index_sequence<TupSize>{},
                                    args...);
  }

public:
  /* this executes the sql function and gets a single result row from it. It
   * fails, if it returns 0 or >1 rows. */
  template <typename T, typename... Ts>
  std::optional<T> query_single(Ts &&...args) {
    if constexpr (BindCount != -1) {
      static_assert(BindCount == sizeof...(Ts));
    }
    if constexpr (is_tuple_like_v<T>) {
      return query_single_tup<T>(args...);
    } else {
      reset();
      if constexpr (sizeof...(args) > 0) {
        if (!bind(std::forward<Ts>(args)...)) {
          return std::nullopt;
        }
      }
      auto rc = sqlite3_step(stmt);
      if (rc != SQLITE_ROW) {
        spdlog::error("Failed to execute statement: {}",
                      sqlite3_errmsg(sqlite3_db_handle(stmt)));
        return std::nullopt;
      }
      m_index = 0;
      T col;
      get_column(col);
      rc = sqlite3_step(stmt);
      if (rc != SQLITE_DONE) {
        spdlog::error("Failed to execute statement: {}",
                      sqlite3_errmsg(sqlite3_db_handle(stmt)));
        return std::nullopt;
      }

      return std::move(col);
    }
  }
};
