trap " kill -9 0 " SIGINTG
node=$1
if [ ! -e "node/geth" ]
then 
	geth --datadir node init genesis.json
fi

geth --gasprice 0 --txpool.pricelimit 0  --mine --datadir node --syncmode 'full' --port 30311 --rpcvhosts '*' --rpc --rpcaddr 0.0.0.0 \
 	--rpcport 8540 --rpcapi 'personal,eth,net,web3,txpool,miner' --rpccorsdomain '*'  \
 	--ws --wsaddr 0.0.0.0 --wsport 8650 --wsapi="personal,eth,net,web3,txpool,miner" --wsorigins "*"  --networkid 1515  \
  	--etherbase '0x3c62aa7913bc303ee4b9c07df87b556b6770e3fc' --unlock '0x3c62aa7913bc303ee4b9c07df87b556b6770e3fc','cb4df655d08c9f054493809dcdb5a47f3a95efaa' --password password.txt    --allow-insecure-unlock  --txpool.globalqueue 2048 --txpool.accountqueue 1024 --targetgaslimit 20000000
