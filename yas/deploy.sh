account=plusplusvote
cleos -u https://rpc.yas.plus set code $account plusplusvote.wasm -p $account@active
cleos -u https://rpc.yas.plus set abi $account plusplusvote.abi -p $account@active