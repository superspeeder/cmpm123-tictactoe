#pragma once

#include <string>
#include <string_view>
#include <tuple>
#include <optional>
#include <vector>
#include <chrono>
#include <fstream>

#if __has_include(<format>)
#define LOGGER_USE_STD_FORMAT
#include <format>
#endif

enum class LogLevel {
    Info = 0,
    Warn = 1,
    Error = 2,
};

struct LogEntry {
    std::chrono::system_clock::time_point timestamp;
    std::string timestamp_string;
    LogLevel log_level;
    std::string message;
    std::string full_text;
    std::optional<std::string> logger_name;
};

#ifdef LOGGER_USE_STD_FORMAT
#define LOGFUNC_HELPER(Ext, Level) inline void Log##Ext(const std::string_view message) { Log(Level, message); }; \
                                   template<class... Args> requires(std::tuple_size_v<std::tuple<Args...>> > 0) void Log##Ext(std::format_string<Args...> fmt, Args&&... args) { Log(Level, fmt, std::forward<Args>(args)...); }

#define LOGFUNC_HELPER_2(Ext, Level, Name) inline void Log##Name##Ext(const std::string_view message) { Log(#Name, Level, message); }; \
                                   template<class... Args> requires(std::tuple_size_v<std::tuple<Args...>> > 0) void Log##Name##Ext(std::format_string<Args...> fmt, Args&&... args) { Log(#Name, Level, fmt, std::forward<Args>(args)...); }
#define LOGFUNC_HELPER_3(Ext, Level, Name, Name2) inline void Log##Name##Ext(const std::string_view message) { Log(#Name2, Level, message); }; \
                                   template<class... Args> requires(std::tuple_size_v<std::tuple<Args...>> > 0) void Log##Name##Ext(std::format_string<Args...> fmt, Args&&... args) { Log(#Name2, Level, fmt, std::forward<Args>(args)...); }
#else
#define LOGFUNC_HELPER(Ext, Level) inline void Log##Ext(const std::string_view message) { Log(Level, message); };
#define LOGFUNC_HELPER_2(Ext, Level, Name) inline void Log##Name##Ext(const std::string_view message) { Log(#Name, Level, message); };
#define LOGFUNC_HELPER_3(Ext, Level, Name, Name2) inline void Log##Name##Ext(const std::string_view message) { Log(#Name2, Level, message); };
#endif

class Logger {
    Logger();

    void Log(const LogEntry& entry);
public:
    static Logger& GetInstance();

    /**
     * @brief Write a basic log message
     * @param level Logging level of the message
     * @param message Logged message
     */
    void Log(LogLevel level, std::string_view message);

    /**
     * @brief Write a log message with a specific logger name
     * @param logger Name of the logger
     * @param level Logging level of the message
     * @param message Logged message
     */
    void Log(std::string_view logger, LogLevel level, std::string_view message);

    #ifdef LOGGER_USE_STD_FORMAT
    /**
     * @brief Write a log message with string formatting via c++20 <format> if available
     */
    template<class... Args> requires(std::tuple_size_v<std::tuple<Args...>> > 0)
    void Log(LogLevel level, std::format_string<Args...> fmt, Args&&... args) {
        const auto message = std::format(fmt, std::forward<Args>(args)...);
        Log(level, message);
    };

    /**
     * @brief Write a log message with string formatting via c++20 <format> if available
     */
    template<class... Args> requires(std::tuple_size_v<std::tuple<Args...>> > 0)
    void Log(const std::string_view logger, LogLevel level, std::format_string<Args...> fmt, Args&&... args) {
        const auto message = std::format(fmt, std::forward<Args>(args)...);
        Log(logger, level, message);
    };
    #endif

    LOGFUNC_HELPER(Info, LogLevel::Info);
    LOGFUNC_HELPER(Warn, LogLevel::Warn);
    LOGFUNC_HELPER(Error, LogLevel::Error);

    LOGFUNC_HELPER_3(Info, LogLevel::Info, GameEvent, GAME);
    LOGFUNC_HELPER_3(Warn, LogLevel::Warn, GameEvent, GAME);
    LOGFUNC_HELPER_3(Error, LogLevel::Error, GameEvent, GAME);


    inline auto begin() { return log_entries.begin(); };
    inline auto end() { return log_entries.end(); };
    inline auto cbegin() const { return log_entries.cbegin(); };
    inline auto cend() const { return log_entries.cend(); };

    inline void Clear() { log_entries.clear(); };


    void UI();
private:
    std::vector<LogEntry> log_entries;
    std::ofstream output_file;
};

#undef LOGFUNC_HELPER
#undef LOGFUNC_HELPER_2
#undef LOGFUNC_HELPER_3