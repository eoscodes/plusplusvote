#include "plusplusvote.hpp"

void plusplusvote::claim(const account_name user)
{
  require_auth(user);

  /* get voter info */
  const auto &voter = _voters.get(user, "unable to find your vote info");
  eosio_assert(voter.proxy == PROXY_ACCOUNT, "you can get rewards when vote yasplusproxy");

  // change user claim info
  auto ct = now();
  auto claimer_itr = _claimer.find(user);
  if (claimer_itr != _claimer.end())
  {
    eosio_assert(ct - claimer_itr->last_claim_time > sec_per_day, "already claimed rewards within past day");
    eosio_assert(ct - claimer_itr->last_violation_time > (3 * sec_per_day), "you should wait 3 days after violation check");
    _claimer.modify(claimer_itr, 0, [&](auto &c) {
      c.last_claim_time = ct;
    });
  }
  else
  {
    claimer_itr = _claimer.emplace(_self, [&](auto &c) {
      c.claimer = user;
      c.last_claim_time = ct;
      c.last_violation_time = 0;
    });
  }

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

  /* send defer check action, cancel within 24 hours */
  eosio::transaction txn{};
  txn.actions.emplace_back(
      eosio::permission_level(_self, N(active)),
      _self,
      N(check),
      std::make_tuple(user));
  txn.delay_sec = _random(user, 60 * 60 * 24);
  txn.send(ct, _self, false);
}

void plusplusvote::check(const account_name user)
{
  require_auth(_self);

  /* check user and next 10  */
  auto ct = now();
  auto claimer_itr = _claimer.find(user);
  if (claimer_itr != _claimer.end())
  {
    auto i = 0;
    while (i++ < 11)
    {
      auto &examinee = _voters.get(claimer_itr->claimer, "unable to find your vote info");
      if (examinee.proxy != PROXY_ACCOUNT)
      {
        _claimer.modify(claimer_itr, 0, [&](auto &c) {
          c.last_violation_time = ct;
        });
      }
      ++claimer_itr;
      if (claimer_itr == _claimer.end())
      {
        claimer_itr = _claimer.begin();
      }
    }
  }
}

void plusplusvote::transfer(const account_name from, const account_name to, const asset quantity, const string memo)
{
  if (from == _self || to != _self)
  {
    return;
  }

  if (memo == "retire")
  {
    action(permission_level{_self, N(active)}, N(pluspluscoin), N(retire), std::make_tuple(quantity, string("retire")))
        .send();
  }
}