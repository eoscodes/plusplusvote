#include <eosiolib/transaction.hpp>
#include <eosiolib/crypto.h>
#include <eosiolib/types.h>
#include <eosiolib/currency.hpp>

#define SYS_SYMBOL S(4, YAS)
#define PLUS_SYMBOL S(4, PLUS)
#define SYSTEM_TOKEN_CONTRACT N(eosio.token)
#define PLUS_TOKEN_CONTRACT N(pluspluscoin)
#define SYSTEM_CONTRACT N(eosio)
#define TEAM_ACCOUNT N(plusplusteam)
#define PROXY_ACCOUNT N(yasplusproxy)
#define REWARD_RATE 0.0001
#define TEAM_RATE 0.2

using namespace eosio;
using namespace std;

class plusplusvote : public eosio::contract
{
public:
    plusplusvote(account_name self) : eosio::contract(self),
                                   _claimer(_self, _self),
                                   _voters(SYSTEM_CONTRACT, SYSTEM_CONTRACT){};

    // @abi action
    void claim(const account_name user);

private:
    struct voter_info
    {
        account_name owner = 0;
        account_name proxy = 0;
        std::vector<account_name> producers;
        int64_t staked = 0;
        double last_vote_weight = 0;
        double proxied_vote_weight = 0;
        bool is_proxy = 0;
        uint32_t reserved1 = 0;
        time reserved2 = 0;
        eosio::asset reserved3;
        uint64_t primary_key() const { return owner; }

        EOSLIB_SERIALIZE(voter_info, (owner)(proxy)(producers)(staked)(last_vote_weight)(proxied_vote_weight)(is_proxy)(reserved1)(reserved2)(reserved3))
    };

    typedef eosio::multi_index<N(voters), voter_info> voters_table;
    voters_table _voters;

    // @abi table claimer i64
    struct claimer_info
    {
        account_name claimer;
        uint64_t last_claim_time = 0;
        uint64_t last_violation_time = 0;
        uint64_t primary_key() const { return claimer; }
        EOSLIB_SERIALIZE(claimer_info, (claimer)(last_claim_time)(last_violation_time))
    };

    typedef eosio::multi_index<N(claimer), claimer_info> claimer_index;
    claimer_index _claimer;

};

EOSIO_ABI(plusplusvote, (claim))
