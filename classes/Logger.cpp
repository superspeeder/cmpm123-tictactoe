#include "classes/Logger.hpp"

#include <iostream>
#include <sstream>

#include "imgui/imgui.h"

constexpr ImVec4 ERROR_COLOR{1.0f, 0.0f, 0.0f, 1.0f};
constexpr ImVec4 WARN_COLOR{1.0f, 1.0f, 0.0f, 1.0f};
constexpr ImVec4 INFO_COLOR{0.0f, 1.0f, 0.0f, 1.0f};

static ImVec4 LOG_COLORS[3] = {
    INFO_COLOR,
    WARN_COLOR,
    ERROR_COLOR,
};

static bool show_log_options = false;


Logger& Logger::GetInstance() {
    static Logger instance;
    return instance;
}

static std::string_view const LEVEL_NAMES[3] = {
    "INFO",
    "WARN",
    "ERROR"
};

static std::string_view const ANSI_LEVEL_COLORS[3] = {
    "\033[32m",
    "\033[33m",
    "\033[31m",
};

static std::string nowstr(std::chrono::system_clock::time_point now) {
    // Not a direct copy but references code in https://stackoverflow.com/questions/77442284/how-can-hours-minutes-and-seconds-be-extracted-from-a-time-point-in-millisecon for extracting time parts from std::chrono::time_point

    const auto days = std::chrono::time_point_cast<std::chrono::duration<int, std::ratio<60 * 60 * 24>>>(now);
    auto time = now - days;

    const auto hours = std::chrono::duration_cast<std::chrono::hours>(time);
    time -= hours;
    const auto minutes = std::chrono::duration_cast<std::chrono::minutes>(time);
    time -= minutes;
    const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(time);
    time -= seconds;
    const auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(time);

    char buf[13];
    buf[12] = 0;
    sprintf(buf, "%02d:%02d:%02d.%03d", (int)(hours.count()), (int)(minutes.count()), (int)(seconds.count()), (int)(millis.count()));
    return buf;
}

Logger::Logger() : output_file("output.log", std::ios::app | std::ios::out) {}

void Logger::Log(const LogEntry &entry) {
    std::cout << ANSI_LEVEL_COLORS[static_cast<int>(entry.log_level)] << entry.full_text << "\033[0m\n";
    output_file << entry.full_text << std::endl;
    log_entries.push_back(entry);
}

static std::string logtext(const LogEntry& entry) {
    std::stringstream text;
    text << '[' << entry.timestamp_string << "] [" << LEVEL_NAMES[static_cast<int>(entry.log_level)] << "] ";
    if (entry.logger_name.has_value()) {
        text << '[' << entry.logger_name.value() << "] ";
    }

    text << entry.message;
    return text.str();
}

void Logger::Log(LogLevel level, const std::string_view message) {
    auto now = std::chrono::system_clock::now();
    LogEntry entry{};
    entry.log_level = level;
    entry.logger_name = std::nullopt;
    entry.timestamp = now;
    entry.timestamp_string = nowstr(now);
    entry.message = message;
    entry.full_text = logtext(entry);
    Log(entry);
}


void Logger::Log(const std::string_view logger, LogLevel level, const std::string_view message) {
    auto now = std::chrono::system_clock::now();
    LogEntry entry{};
    entry.log_level = level;
    entry.logger_name = logger;
    entry.timestamp = now;
    entry.timestamp_string = nowstr(now);
    entry.message = message;
    entry.full_text = logtext(entry);
    Log(entry);
}


static void LoggerOptions() {
    if (ImGui::Begin("Log Options", &show_log_options)) {
        if (ImGui::CollapsingHeader("Output Colors")) {
            ImGui::ColorEdit3("Info", &LOG_COLORS[static_cast<int>(LogLevel::Info)].x);
            ImGui::ColorEdit3("Warn", &LOG_COLORS[static_cast<int>(LogLevel::Warn)].x);
            ImGui::ColorEdit3("Error", &LOG_COLORS[static_cast<int>(LogLevel::Error)].x);
        }
    }
    ImGui::End();
}


static void LogEntryUI(const LogEntry& entry) {
    ImVec4 col = LOG_COLORS[static_cast<int>(entry.log_level)];
    ImGui::TextColored(col, entry.full_text.data());
}

void Logger::UI() {
    if (ImGui::Begin("Game Log")) {
        if (ImGui::Button("Options")) {
            show_log_options = true;
        }

        ImGui::SameLine();

        if (ImGui::Button("Clear")) {
            Logger::GetInstance().Clear();
        }

        ImGui::SameLine();

        if (ImGui::Button("Test Info")) {
            Logger::GetInstance().LogInfo("Hello Info!");
        }

        ImGui::SameLine();

        if (ImGui::Button("Test Warning")) {
            Logger::GetInstance().LogWarn("Hello Warning!");
        }

        ImGui::SameLine();

        if (ImGui::Button("Test Error")) {
            Logger::GetInstance().LogError("Hello Error!");
        }

        ImGui::Separator();

        if (ImGui::BeginChild("Game Log|LogOut", ImGui::GetContentRegionAvail(), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar)) {
            for (const auto& entry : Logger::GetInstance()) {
                LogEntryUI(entry);
            }
        }
        ImGui::EndChild();
    }

    ImGui::End();

    if (show_log_options) {
        LoggerOptions();
    }
}