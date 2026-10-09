#include <cstddef>
#include <optional>
#include <string>
#include <vector>

struct LogEntry {
    std::string command;
    int term;
};

struct State {
    int currentTerm = 0;
    std::optional<int> votedFor;
    std::vector<LogEntry> log;
    int commitIndex = 0;
    int lastApplied = 0;
    std::vector<int> nextIndex;
    std::vector<int> matchIndex;

    void initializeLeaderState(std::size_t serverCount) {
        const int nextLogIndex = static_cast<int>(log.size()) + 1;
        nextIndex.assign(serverCount, nextLogIndex);
        matchIndex.assign(serverCount, 0);
    }
};