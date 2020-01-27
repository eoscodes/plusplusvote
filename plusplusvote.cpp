#include "plusplusvote.hpp"

void plusplusvote::claim(const account_name user)
{
    require_auth(user);

    /* get voter info */
    const auto &voter = _voters.get(user, "unable to find your vote info");
    eosio_assert(voter.proxy == PROXY_ACCOUNT, "you can get rewards when vote yasplusproxy");
    auto reward = asset(voter.staked * REWARD_RATE, PLUS_SYMBOL);
    auto team_reward = asset(voter.staked * REWARD_RATE * TEAM_RATE, PLUS_SYMBOL);
    auto need_issue = reward + team_reward;
    // issue
    action(permission_level{_self, N(active)}, PLUS_TOKEN_CONTRACT, N(issue),
           std::make_tuple(_self, need_issue, std::string("issue for reward")))
        .send();
    // send to user
    action(permission_level{_self, N(active)}, PLUS_TOKEN_CONTRACT, N(transfer),
           std::make_tuple(_self, user, reward, std::string("reward for vote")))
        .send();
    // send to team
    action(permission_level{_self, N(active)}, PLUS_TOKEN_CONTRACT, N(transfer),
           std::make_tuple(_self, TEAM_ACCOUNT, team_reward, std::string("reward for team")))
        .send();
}
