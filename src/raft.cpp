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

struct RequestVoteArgs {
    int term;
    int candidateId;
    int lastLogIndex;
    int lastLogTerm;
};

struct RequestVoteResults {
    int term;
    bool voteGranted;
};

RequestVoteResults handleRequestVote(const RequestVoteArgs& args, State& state) {
    RequestVoteResults response;
    response.term = state.currentTerm;

    if (args.term < state.currentTerm) {
        response.voteGranted = false;
        return response;
    }

    if (args.term > state.currentTerm) {
        state.currentTerm = args.term;
        // Reset votedFor when the term is updated to indicate that the server has not voted for any candidate in the new term.
        state.votedFor.reset();
    }

    // Check if the candidate's log is at least as up-to-date as the receiver's log
    // A log is considered up-to-date if its last log term is greater than the receiver's last log term,
    // or if the last log terms are equal, the candidate's last log index is greater than or equal to the receiver's last log index.
    bool logUpToDate = (args.lastLogTerm > (state.log.empty() ? 0 : state.log.back().term)) ||
                       (args.lastLogTerm == (state.log.empty() ? 0 : state.log.back().term) &&
                        args.lastLogIndex >= static_cast<int>(state.log.size()));

    if ((!state.votedFor.has_value() || state.votedFor.value() == args.candidateId) && logUpToDate) {
        state.votedFor = args.candidateId;
        response.voteGranted = true;
    } else {
        response.voteGranted = false;
    }

    return response;
}