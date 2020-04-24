const Web3 = require('web3');
const provider = new Web3.providers.HttpProvider("http://localhost:7545");
const web3 = new Web3(provider)
const contract = require('@truffle/contract');
const aztecJSON = require('./build/contracts/AZTEC.json');
const aztec = contract(aztecJSON);
aztec.setProvider(provider);
const accounts = web3.eth.getAccounts()

async function test() {
	const accounts = await web3.eth.getAccounts()

	aztec.defaults({
		from: accounts[0]

	})
	let instance = await aztec.deployed();
	let s = await instance.verify_move_in(
		"0x24479288d0e3414ddb768d2e34a906d1595476bc29ad2d821adff7ce93db0e5327cf6a8d649fd793dd5f345078faeffa4a500908e63f1c133861939695a57dc229430299e1c9857d618cbbf4e67f37b5cc88b7819784ee79abe5ada3840182390de77c8fef13f98ba5edf5a1845515902e9223a4aef62d4054e3ef2f8ea0167414d0e0af7580b5f90d36aad9037c2d4ea7b05711bb1b9f46e87a042bec5829d12e21e4916a0356fddbfb9d37ba657451227e45122417ec84300994d2888ad6990d3318aab6048dd91d219895aa728669a62ef8fe9ad632656e4cfa90cb88f84525085a0337a91132e730ffb210083377c95f69febfb5e1585271b93e3edbdadc", "0x0ce891f03343f5ac1249a1ef1bd4d473b37515446170c18efdfc1a3d79b7e1f92efb02bcaffb92a98f3f6517f59cb14781348c6e08901524bf3d8966f70849f713be3fd822c3efcade2e65639ebdc2fec6b527499b9e7cb940f66d8a3a5258b11668690b9de55a2f057aa9913af2c66cd74c56f7e45461d8a86f0a47b85098de06e7d3a1e70f5d3753913f183fd7d7c3fe156dcfd767ffbc0836d6930fdda7231a059df3c667482f1b0e891af8c1207032da905c32640705baa2477bcb13bdd915d52ffdddb36d5782956213884e65f6737c033f5fbca0ba8f574e93043d51f110d9dc3e9aa08128517dc488acdde02555576bce4b144cd5cc3227a1e4b4a1cc", 2, 4, web3.utils.toBN("0x80ea49eb72a907184104138f088afce2455fa3448508466e4521ad16bd1616e"), "0x022f2a681375e98a2ab2dcd63fc5aba13d5b1eba0b370348152d63f5b8e2a5a327bb233387ae6edfee0b72fdc0bb98e3ca5d220f90bc70fdc27034b2f7579c750a27a6114819cb51db53e105114496ce32df291f71d3e6c67ffc8c6baf57216c09d31797fbb6e8de13d627458eb6907cd8e07de71efc692a9a09c10a01655bff", "0x15deffc3cab7f2c6b860e318f1923436f8adc7f97ed0d1e844e1d0d11b432f242bcebff6c4a88797951b78dcae6a5dc1e80f972b06f5bf1546ea079f4ee4876c2547376b9410bb5df3be7be9494787e2324ba90c8f69c09f60d0d7507853d587", 4, {
			from: accounts[0]
		})
	console.log("verification status", s.receipt.status)
	console.log("gas used", s.receipt.gasUsed);
	// aztec.deployed().then(instance =>
	// 	instance.test()
	// ).then(
	// 	s => console.log(s)
	// );
}

test()

// module.exports = async function(callback) {
// 	// perform actions
// 	const aztec = artifacts.require("AZTEC");
// 	let accounts = await web3.eth.getAccounts()
// 	// console.log(aztec.deployed())
// 	let instance = await aztec.deployed()
// 	// instance.verify_move_in("test").then(() => console.log("test"))
// let s = await instance.verify_move_in.estimateGas("0x1a1747c38555020b12d31b4d9ecc02a15175dfae0297cd1ab5e5ba04fa9dcc2a0f77486530feb7c3ce0416f19f2821d14dd560dff42ec45b907389239c94b8602c1d3f8154751c8e4c64dbfbdd2c3ebe5b6319cf3b2ab9b5d69008c763155f18002fa0478447dcdc460651a43fe194ac6c76e6135197a1fad30da19ff06a6891", "0x1b0b7e2e1644837c692e44b07bcf53c725a0745a8e84f846145551fe8a39e5ef07578b9cea889c322163d5b6553673c6847af54b3b3afd5fe2662bbf825a66e92e8f687252db52ada21ca009096c95163af88783994e9bf1384dd51e5022be7b0137467e9a0913d4556752ca220d20b27a93742a26099ac4d7a4f39fd97aa32f", 2, 1, 3741674376036272320099815359845886074519930458677647374460495493377186811077, "0x2b55b776e5afbe8f18798407267550ded586bfccb8121e067553beea1d98f83c074fb79129484036ee0820815be420badc011d7fdc963d0ebc05d2c07d0c75b2", "0x28d199576b0caab51392669f7bfc8a56a00d1ea345efe022af3808b5932961fe", 2, {
// 	from: accounts[0]
// })
// 	console.log(instance);
// 	s = await instance.verify_move_in({
// 		from: accounts[0]
// 	});
// 	console.log(s);

// 	// s = await instance.verify_move_in.estimateGas("0x1a1747c38555020b12d31b4d9ecc02a15175dfae0297cd1ab5e5ba04fa9dcc2a0f77486530feb7c3ce0416f19f2821d14dd560dff42ec45b907389239c94b8602c1d3f8154751c8e4c64dbfbdd2c3ebe5b6319cf3b2ab9b5d69008c763155f18002fa0478447dcdc460651a43fe194ac6c76e6135197a1fad30da19ff06a6891", "0x1b0b7e2e1644837c692e44b07bcf53c725a0745a8e84f846145551fe8a39e5ef07578b9cea889c322163d5b6553673c6847af54b3b3afd5fe2662bbf825a66e92e8f687252db52ada21ca009096c95163af88783994e9bf1384dd51e5022be7b0137467e9a0913d4556752ca220d20b27a93742a26099ac4d7a4f39fd97aa32f", 2, "3741674376036272320099815359845886074519930458677647374460495493377186811077,0x2b55b776e5afbe8f18798407267550ded586bfccb8121e067553beea1d98f83c074fb79129484036ee0820815be420badc011d7fdc963d0ebc05d2c07d0c75b2", "0x28d199576b0caab51392669f7bfc8a56a00d1ea345efe022af3808b5932961fe", 2, {
// 	// 	from: accounts[0]
// 	// })
// 	// console.log(s);
// 	// console.log("tttt");
// 	callback();
// }